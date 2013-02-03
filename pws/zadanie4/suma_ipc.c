#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "semun.h"
#include "err.h"

#include "suma_synchr.h"
#include "suma_ipc.h"
#include "common.h"



void sp_uruchomienieProcesow(int ile, 
			     int kolSys,
			     int semStart,
			     int dlugoscLiczby,
			     int ileLiczb,
			     int dlugoscKom,
			     char **liczby)
{
  int i, j;
  pid_t pid;
  char paczka[ileLiczb + 1], numer[6], ileProc[6], maxPrzen[3], dlKom[5];
  union semun su;
  komSys kom;
  
  /* wyzerowanie semafora startowego                                    */
  su.val = 0;
  if (semctl(semStart, 0, SETVAL, su) == -1)
    syserr("semctl (SETVAL)");

  paczka[ileLiczb] = 0;
  sprintf(ileProc, "%d", ile);
  sprintf(maxPrzen, "%d", ile - dlugoscLiczby + 1);
  sprintf(dlKom, "%d", dlugoscKom);

  for (i = 0; i < ile; i++) {
    sprintf(numer, "%d", i);
    for (j = 0; j < ileLiczb; j++)
      if (i < dlugoscLiczby)
	paczka[j] = liczby[j][dlugoscLiczby - i - 1];
      else
	paczka[j] = '0';

    switch (pid = fork()) {
      case -1:
	syserr("fork");
      case 0:
	execlp("./obliczeniowy_ipc", "obliczeniowy_ipc", numer, ileProc, maxPrzen, dlKom, paczka, 0);
	syserr("execlp");
      default:
	break;
    }
    if (msgrcv(kolSys, &kom, KK_DANE_SYS, KSYS_START, 0) == -1)
      syserr("msgrcv (KSYS_START)");
  }
}


void sp_startObliczen(int semStart)
{
  union semun su;

  TRACE("START");
  /* usuniecie semafora startowego                                      */
  if (semctl(semStart, 0, IPC_RMID, su) == -1)
    syserr("semctl (IPC_RMID)");
}

void sp_czekanieNaKoniec(int ile, int kolSys)
{
  komSys kom;
  int i;

  for (i = 0; i < ile; i++)
    if (msgrcv(kolSys, &kom, KK_DANE_SYS, KSYS_POLICZONE, 0) == -1)
      syserr("msgrcv (KSYS_POLICZONE)");
}

void sp_zebranieWynikow(int ileProcesow, int ileLiczb, int kolCyfry, int kolSys)
{
  int i, j, start, koniec;
  char **wynik;
  int *paczki;
  int skonczyli = 0;
  komCyfry kc;
  komSys ks;

  CALLOC(wynik, ileLiczb, sizeof(char *));
  for (i = 0; i < ileLiczb; i++)
    MALLOC(wynik[i], ileProcesow);
  CALLOC(paczki, ileProcesow, sizeof(int));

  /* odbieranie paczek z wynikami                                       */
  while (skonczyli < ileProcesow) {
    if (msgrcv(kolCyfry, &kc, KK_DANE_CYFRY, 0, 0) == -1)
      syserr("msgrcv (kolCyfry)");
    
    start = paczki[(int)kc.numer - 1] * KK_DANE_CYFRY;
    koniec = ileLiczb - paczki[(int)kc.numer - 1] * KK_DANE_CYFRY;
    if (koniec > KK_DANE_CYFRY)
      koniec = KK_DANE_CYFRY;

    for (i = 0; i < koniec; i++)
      wynik[start + i][(int)kc.numer - 1] = kc.cyfry[i];

    if((++paczki[(int)kc.numer - 1]) == ileLiczb / KK_DANE_CYFRY + 1)
      skonczyli++;
  }

  for (i = 0; i < ileProcesow; i++) {
    ks.typ = KSYS_SKONCZ;
    if (msgsnd(kolSys, &ks, KK_DANE_SYS, 0) == -1)
      syserr("msgsnd (kolSys)");
    if (msgrcv(kolSys, &ks, KK_DANE_SYS, KSYS_KONIEC, 0) == -1)
      syserr("msgrcv (KSYS_KONIEC)");
  }

  /* usuniecie kolejek komunikatow                                      */
  if (msgctl(kolSys, IPC_RMID, 0) == -1)
    syserr("msgctl (kolSys: IPC_RMID)");
  if (msgctl(kolCyfry, IPC_RMID, 0) == -1)
    syserr("msgctl (kolCyfry: IPC_RMID)");

#ifndef BEZ_WYNIKOW
  for (i = 0; i < ileLiczb; i++) {
    for (j = ileProcesow - 1; j >=0; j--)
      printf("%c", wynik[i][j]);
    printf("\n");
  }
#endif
}









