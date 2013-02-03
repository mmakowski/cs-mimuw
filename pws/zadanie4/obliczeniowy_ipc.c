#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "suma_synchr.h"
#include "err.h"
#include "semun.h"
#include "common.h"
#include "common_ipc.h"
#include "suma_ascii.h"

#define BRAK -1
#define NOWY 'n'
#define STARY 's'

#define BAZA(i) ((i) * (maxPrzen + 2))

#ifdef MK_KOPIUJ
#define KOM_KOPIUJ \
for (j = 0; j < rozmKom; j++) \
kom[j] = pBuf[ileLiczb * (maxPrzen + 2) + j];
#define KOM_ZMIENNE char* kom;  
#define KOM_ZMIENNE_INI \
MALLOC(kom, rozmKom);
#endif

#ifndef MK_KOPIUJ
#define KOM_ZMIENNE
#define KOM_ZMIENNE_INI
#define KOM_KOPIUJ
#endif

int main(int argc, char **argv)
{
  int i, j, koniec;
  int semStart, kolSys, kolCyfry;
  int bPrawy, bLewy, semLewy, semPrawy, semLewyNowe, semPrawyNowe, semLNum = BRAK, semPNum = BRAK;
  int numer, maxProc, maxPrzen, ileLiczb, ileSk = 0, rozmKom;
  int nowa = BRAK;
  char *lBuf, *pBuf;
  struct sembuf sb;
  struct shmid_ds sids;
  union semun su;
  komSys ks;
  komCyfry kc;
  char **bufor;
  KOM_ZMIENNE

  ileLiczb = strlen(argv[5]);
  numer = atoi(argv[1]);
  maxProc = atoi(argv[2]) - 1;
  maxPrzen = atoi(argv[3]);
  rozmKom = atoi(argv[4]);

  KOM_ZMIENNE_INI

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
    SEMGET(semPrawy, BAZA_SEM_KEY + ((numer - 1) / SEM_W_ZESTAWIE) * 2, SEM_W_ZESTAWIE, 0);
    SEMGET(semPrawyNowe, BAZA_SEM_KEY + ((numer - 1) / SEM_W_ZESTAWIE) * 2 + 1, SEM_W_ZESTAWIE, 0);
    semPNum = (numer - 1) % SEM_W_ZESTAWIE;
    SHMGET(bPrawy, BAZA_SHM_KEY + numer - 1, (maxPrzen + 2) * ileLiczb + rozmKom, 0);
    SHMAT(pBuf, bPrawy);
  }

  if (numer < maxProc) {
    if (numer % SEM_W_ZESTAWIE == 0) {
      SEMGET(semLewy, BAZA_SEM_KEY + (numer / SEM_W_ZESTAWIE) * 2, SEM_W_ZESTAWIE, 0600 | IPC_CREAT | IPC_EXCL);
      SEMGET(semLewyNowe, BAZA_SEM_KEY + (numer / SEM_W_ZESTAWIE) * 2 + 1, SEM_W_ZESTAWIE, 0600 | IPC_CREAT | IPC_EXCL);
      su.val = 1;
      for (i = 0; i < SEM_W_ZESTAWIE; i++)
	SEMCTL(semLewy, i, SETVAL, su);
      su.val = 0;
      for (i = 0; i < SEM_W_ZESTAWIE; i++)
	SEMCTL(semLewyNowe, i, SETVAL, su);
      semLNum = 0;
    } else {
      SEMGET(semLewy, BAZA_SEM_KEY + (numer / SEM_W_ZESTAWIE) * 2, SEM_W_ZESTAWIE, 0);
      SEMGET(semLewyNowe, BAZA_SEM_KEY + (numer / SEM_W_ZESTAWIE) * 2 + 1, SEM_W_ZESTAWIE, 0);
      semLNum = numer % SEM_W_ZESTAWIE;
    }
    SHMGET(bLewy, BAZA_SHM_KEY + numer, (maxPrzen + 2) * ileLiczb + rozmKom, 0600 | IPC_CREAT | IPC_EXCL);
    SHMAT(lBuf, bLewy);
    for (i = 0; i < ileLiczb; i++) {
      for (j = 0; j < maxPrzen; j++)
	lBuf[BAZA(i) + j] = '0';
      lBuf[BAZA(i) + maxPrzen] = 0;
      lBuf[BAZA(i) + maxPrzen + 1] = STARY;
    }
  }

  /* sygnalizacja gotowosci do startu                                        */
  ks.typ = KSYS_START;
  if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
    syserr("obl: msgsnd (KSYS_START)");

  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;  
  semop(semStart, &sb, 1);

  /* sumowanie                                                               */
  for (i = 0; i < ileLiczb - 1; i++)
    suma(bufor[i], bufor[i + 1]);

  while (ileSk < ileLiczb) {
    if (numer > 0) {
      SEM_P(semPrawyNowe, semPNum);
      SEM_P(semPrawy, semPNum);
      nowa = BRAK;
      i = 0;
      while ((nowa == BRAK) && (i < ileLiczb)) {
	if (pBuf[BAZA(i) + maxPrzen + 1] == NOWY) {
	  nowa = i;
	  suma(pBuf + BAZA(i), bufor[i]);
	  pBuf[BAZA(i) + maxPrzen + 1] = STARY;
	  KOM_KOPIUJ
	}
	i++;
      }
      SEM_V(semPrawy, semPNum);
      if (nowa != BRAK) {
	if (numer < maxProc) {
	  SEM_P(semLewy, semLNum);
	  for (j = maxPrzen - 2; j >= 0; j--)
	    lBuf[BAZA(nowa) + j + 1] = bufor[nowa][j];
	  lBuf[BAZA(nowa) + maxPrzen + 1] = NOWY;
	  for (j = 0; j < rozmKom; j++)
	    lBuf[BAZA(ileLiczb) + j] = 'k';
	  SEM_V(semLewy, semLNum);
	  SEM_V(semLewyNowe, semLNum);
	}
	ileSk++;
      } 
    } else 
      for (i = 0; i < ileLiczb; i++) {
	SEM_P(semLewy, semLNum);
        for (j = maxPrzen - 2; j >= 0; j--)
	  lBuf[BAZA(i) + j + 1] = bufor[i][j];
	lBuf[BAZA(i) + maxPrzen + 1] = NOWY;
	for (j = 0; j < rozmKom; j++)
	  lBuf[BAZA(ileLiczb) + j] = 'k';
	SEM_V(semLewy, semLNum);
	SEM_V(semLewyNowe, semLNum);
	ileSk++;
      }    
  }

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
  if (semLNum == 0) {
    SEMCTL(semLewy, 0, IPC_RMID, su);
    SEMCTL(semLewyNowe, 0, IPC_RMID, su);
  }
  SHMDT((void *)lBuf);
  SHMDT((void *)pBuf);
  if (numer < maxProc)
    SHMCTL(bLewy, IPC_RMID, &sids);

  return 0;
}









