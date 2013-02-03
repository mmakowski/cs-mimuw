#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "mesg.h"
#include "err.h"


#define MIN(x, y) ((x) < (y) ? (x) : (y))

static void opisProgramu()
{
  printf("ipccat (c) 2001 Maciek Makowski\n\n");
  printf("program wypisuje zawartosc pliku na standardowe wyjscie.\n\n");
  printf("\tSposob uzycia: ipccat n nazwa_pliku\n\n");
  printf("n           - liczba agentow odczytujacych zawartosc pliku (n > 0)\n");
  printf("nazwa_pliku - plik do wypisania\n\n");

  exit(0);
}


static void msgrm(const int *kol)
{
  int i;

  for (i = 0; i < 4; i++)
    if (msgctl(kol[i], IPC_RMID, 0) == -1)
      fprintf(stderr, "ERROR: msgctl (RMID)\n");
}


void rmsyserr(const int *kol, const char *kom)
{
  msgrm(kol);
  syserr(kom);
}

int main(int argc, char **argv)
{
  int i, j, ileAg, ileBlokow, dlugosc;
  int kol[4];
  pid_t pid;
  komSys komSystem;
  komSynch komSynchro;
  komCat komBlok;

  if (argc < 3)
    opisProgramu();

  if ((ileAg = atoi(argv[1])) < 1)
    opisProgramu();

  if ((kol[SYS] = msgget(KOL_SYS, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("msgget (kolSys)");
  if ((kol[CAT] = msgget(KOL_CAT, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("msgget (kolCat)");
  if ((kol[CZYT] = msgget(KOL_CZYT, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("msgget (kolCzyt)");
  if ((kol[PISZ] = msgget(KOL_PISZ, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
    syserr("msgget (kolPisz)");

  komSystem.typ = KS_DAJ_LIMIT;
  if (msgsnd(kol[SYS], &komSystem, 0, 0) != 0)
    rmsyserr(kol, "msgsnd (kolSys: DAJ_LIMIT)");

  for (i = 0; i < ileAg; i++)
    switch (pid = fork()) {
      case -1:
        rmsyserr(kol, "fork");

      case 0:
        execlp("./ipccatagent", "ipccatagent", argv[2], 0);
        rmsyserr(kol, "exec");

      default:
        break;
    }

  komSynchro.typ = 1L;
  if (msgsnd(kol[PISZ], &komSynchro, 0, 0) != 0)
    rmsyserr(kol, "msgsnd (kolPisz)");

  /* wczytywanie liczby blokow do odebrania                           */
  if (msgrcv(kol[SYS], &komSystem, MAX_SYS_DANE, KS_LIMIT, 0) <= 0)
    rmsyserr(kol, "msgrcv (kolSys: LIMIT)");

  if ((ileBlokow = atoi(komSystem.dane)) >= 0) {
    /* ustawianie zadan poczatkowych                                  */
    komSystem.typ = KS_ODBIERZ;
    for (j = 1; j <= MIN(ileBlokow, ileAg); j++) {
      komSynchro.typ = (long)j;
      if (msgsnd(kol[CZYT], &komSynchro, 0, 0) != 0)
        rmsyserr(kol, "msgsnd (kolCzyt)");
      if (msgsnd(kol[SYS], &komSystem, 0, 0) != 0)
        rmsyserr(kol, "msgsnd (kolSys: ODBIERZ)");
    }

    /* wczytywanie po kolei blokow z kolCat                           */
    for (i = 1; i <= ileBlokow; i++) {
      if ((dlugosc = msgrcv(kol[CAT], &komBlok, ROZMIAR_BLOKU, 0L, 0)) <= 0)
        rmsyserr(kol, "msgrcv (kolCat)");
      write(1, komBlok.dane, dlugosc);
      if (j + i - 1 <= ileBlokow) {
        komSynchro.typ = j + i - 1;
        if (msgsnd(kol[CZYT], &komSynchro, 0, 0) != 0)
          rmsyserr(kol, "msgsnd (kolCzyt)");
        if (msgsnd(kol[SYS], &komSystem, 0, 0) != 0)
          rmsyserr(kol, "msgsnd (kolSys: ODBIERZ)");
      }
    }
  } else
    printf("Nie mozna otworzyc pliku '%s'\n", argv[2]);


  /* wysylanie polecenia zakonczenia dzialania do agentow             */
  komSystem.typ = KS_GIN;
  if (msgsnd(kol[SYS], &komSystem, 0, 0) != 0)
    rmsyserr(kol, "msgsnd (kolSys: GIN)");

  for (i = 0; i < ileAg; i++)   /* oczekiwanie na zakonczenie agentow */
    if (msgrcv(kol[SYS], &komSystem, 0, KS_ZGINALEM, 0) == -1)
      rmsyserr(kol, "msgrcv (kolSys: ZGINALEM)");
    
  msgrm(kol);
  return 0;
}







