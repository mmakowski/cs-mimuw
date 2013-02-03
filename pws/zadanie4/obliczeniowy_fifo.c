#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <fcntl.h>

#include "semun.h"
#include "suma_synchr.h"
#include "err.h"
#include "common.h"
#include "suma_ascii.h"
#include "common_ipc.h"

#define FIFO_NAZWA "fifo_sum"
#define FIFO_NAZWA_DL 20


int main(int argc, char **argv)
{
  int i, j, koniec;
  int semStart, kolSys, kolCyfry;
  int numer, maxProc, maxPrzen, ileLiczb, rozmKom;
  struct sembuf sb;
  komSys ks;
  komCyfry kc;
  char **bufor;
  char *fifoPNazwa = 0, *fifoLNazwa = 0;
  int fifoPD = -1, fifoLD = -1;
  char *msgL, *msgP;

  ileLiczb = strlen(argv[5]);
  numer = atoi(argv[1]);
  maxProc = atoi(argv[2]) - 1;
  maxPrzen = atoi(argv[3]);
  rozmKom = atoi(argv[4]);

  /* kopiowanie cyfr wejsciowych do bufora                                   */
  CALLOC(bufor, ileLiczb, sizeof(char *));
  for (i = 0; i < ileLiczb; i++) {
    MALLOC(bufor[i], maxPrzen + 1);
    for (j = 0; j < maxPrzen - 1; j++)
      bufor[i][j] = '0';
    bufor[i][maxPrzen - 1] = argv[5][i];
    bufor[i][maxPrzen] = 0;
  }
  
  if ((kolSys = msgget(KOL_KEY_SYS, 0)) == -1)
    syserr("obl: msgget (kolSys)");
  if ((kolCyfry = msgget(KOL_KEY_CYFRY, 0)) == -1)
    syserr("obl: msgget (kolCyfry)");
  if ((semStart = semget(SEM_KEY_START, 1, 0)) == -1)
    syserr("obl: semget");

  /* przygotowanie narzedzi do komunikacji                                   */
  if (numer > 0) {
    ks.typ = KSYS_START;
    if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
      syserr("obl: msgsnd (KSYS_START)");

    MALLOC(fifoPNazwa, FIFO_NAZWA_DL);
    sprintf(fifoPNazwa, "%s%04d", FIFO_NAZWA, numer); 
    if (mknod(fifoPNazwa, S_IFIFO | 0600, 0) != 0)
      syserr("mknod");
    if ((fifoPD = open(fifoPNazwa, O_RDONLY)) == -1)
      syserr("open");
  }

  if (numer < maxProc) {
    MALLOC(fifoLNazwa, FIFO_NAZWA_DL);
    sprintf(fifoLNazwa, "%s%04d", FIFO_NAZWA, numer + 1); 
    if ((fifoLD = open(fifoLNazwa, O_WRONLY)) == -1)
      syserr("open");
  }

  if (numer == 0) {
  /* sygnalizacja gotowosci do startu                                        */    ks.typ = KSYS_START;
    if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
      syserr("obl: msgsnd (KSYS_START)");
  }

  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;  
  semop(semStart, &sb, 1);

  /* sumowanie                                                               */
  MALLOC(msgP, maxPrzen + 1 + rozmKom);
  MALLOC(msgL, maxPrzen + 1 + rozmKom);
  msgL[0] = '0';

  if (numer > 0) {
    for (i = 0; i < ileLiczb; i++) {
      if (i < ileLiczb - 1)
	suma(bufor[i], bufor[i + 1]);
      if (read(fifoPD, msgP, maxPrzen + 1 + rozmKom) == -1)
	syserr("read (fifoPD)");
      suma(msgP, bufor[i]);
      if (numer < maxProc) {
	for (j = 0; j < maxPrzen - 1; j++)
	  msgL[j + 1] = bufor[i][j];
	msgL[maxPrzen] = 0;
	if(write(fifoLD, msgL, maxPrzen + 1 + rozmKom) == -1)
	  syserr("write");
      }
    }
  } else {
    for (j = 0; j < maxPrzen - 1; j++)
      msgL[j + 1] = bufor[0][j];
    msgL[maxPrzen] = 0;
    if(write(fifoLD, msgL, maxPrzen + 1 + rozmKom) == -1)
      syserr("write");

    for (i = 0; i < ileLiczb - 1; i++) {
      suma(bufor[i], bufor[i + 1]);
      for (j = 0; j < maxPrzen - 1; j++)
	msgL[j + 1] = bufor[i + 1][j];
      msgL[maxPrzen] = 0;
      if(write(fifoLD, msgL, maxPrzen + 1 + rozmKom) == -1)
	syserr("write");
    }
  }
  
  if (numer > 0)
    if (close(fifoPD) != 0)
      syserr("close");
  if (numer < maxProc)
    if (close(fifoLD) != 0)
      syserr("close");

  ks.typ = KSYS_POLICZONE;
  if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
    syserr("obl: msgsnd (KSYS_POLICZONE)");

  /* wysylanie wyniku                                                        */
  kc.numer = numer + 1; 
  for (i = 0; i < ileLiczb / KK_DANE_CYFRY + 1; i++) {
    koniec = ileLiczb - i * KK_DANE_CYFRY;
    if (koniec > KK_DANE_CYFRY)
      koniec = KK_DANE_CYFRY;
    for (j = 0; j < koniec; j++)
      kc.cyfry[j] = bufor[i * KK_DANE_CYFRY + j][maxPrzen - 1];

    if (msgsnd(kolCyfry, &kc, KK_DANE_CYFRY, 0) == -1)
      syserr("obl: msgsnd (kolCyfry)");
  }
  
  MSGRCV(kolSys, &ks, KK_DANE_SYS, KSYS_SKONCZ);
  ks.typ = KSYS_KONIEC;
  if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
    syserr("obl: msgsnd (KSYS_KONIEC)");
  
  /* zwalnianie zasobow                                                      */
  if (numer > 0) {
    if (unlink(fifoPNazwa) != 0)
      syserr("unlink");
  }
  return 0;
}









