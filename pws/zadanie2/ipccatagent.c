#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "mesg.h"
#include "err.h"


static void opisProgramu()
{
  printf("ipccatagent (c) 2001 Maciek Makowski\n\n");
  printf("agent programu ipccat odczytujacy bloki z podanego pliku.\n\n");
  printf("\tSposob uzycia: ipccatagent nazwa_pliku\n\n");
  printf("nazwa_pliku - plik do wczytywania\n\n");

  exit(0);
}

static int wczytajBlok(int plik, long nrBloku, komCat *komBlok)
{
  lseek(plik, ROZMIAR_BLOKU * (nrBloku - 1), SEEK_SET);
  return (int)read(plik, komBlok->dane, ROZMIAR_BLOKU);  
}

int main(int argc, char **argv)
{
  int kolSys, kolCat, kolCzyt, kolPisz;
  int plik, dlugosc;
  long nrBloku, ileBlokow;
  komSys komSystem;
  komSynch komSynchro;
  komCat komBlok;
  struct stat atrPliku;
  unsigned char koncz = 0, brak = 0;

  if (argc < 2)
    opisProgramu();

  if ((kolSys = msgget(KOL_SYS, 0)) == -1)
    syserr("msgget (kolSys)");
  if ((kolCat = msgget(KOL_CAT, 0)) == -1)
    syserr("msgget (kolCat)");
  if ((kolCzyt = msgget(KOL_CZYT, 0)) == -1)
    syserr("msgget (kolCzyt)");
  if ((kolPisz = msgget(KOL_PISZ, 0)) == -1)
    syserr("msgget (kolPisz)");

  if ((plik = open(argv[1], 0)) == -1)
    brak = 1;

  /* obliczanie sumarycznej liczby blokow do wczytania                 */
  if (msgrcv(kolSys, &komSystem, 0, KS_DAJ_LIMIT, IPC_NOWAIT) != -1) {
    if (brak == 1)
      ileBlokow = -1;
    else {
      fstat(plik, &atrPliku);
      ileBlokow = (long)((double)atrPliku.st_size / (double)ROZMIAR_BLOKU);
      if (((double)atrPliku.st_size / (double)ROZMIAR_BLOKU) > (double)ileBlokow)
	ileBlokow++;
    }
    sprintf(komSystem.dane, "%ld", ileBlokow);
    komSystem.typ = KS_LIMIT;
    if (msgsnd(kolSys, &komSystem, strlen(komSystem.dane), 0) == -1)
      syserr("msgsnd (kolSys: LIMIT)");
  }

  komBlok.typ = KC_BLOK;

  while (!koncz) {
    while (msgrcv(kolSys, &komSystem, 0, -KS_GIN, 0) == -1);
    /* bez obslugi bledu. Dzieki temu program bedzie kontynuowal       *
     * dzialanie po otrzymaniu SIGSTOP w trakcie wykonywania powyzszej *
     * instrukcji.                                                     */
    switch (komSystem.typ) {
      case KS_GIN:
	if (msgsnd(kolSys, &komSystem, 0, 0) == -1)
	  syserr("msgsnd (kolSys: GIN)");
	koncz = 1;
	break;
      case KS_ODBIERZ:
	if (msgrcv(kolCzyt, &komSynchro, 0, 0L, 0) == -1)
	  syserr("msgrcv (kolCzyt)");
	nrBloku = komSynchro.typ;
	if ((dlugosc = wczytajBlok(plik, nrBloku, &komBlok)) == 0) {
	  komSystem.typ = KS_KONIEC;
	  if (msgsnd(kolSys, &komSystem, 0, 0) == -1)
	    syserr("msgsnd (kolSys: KONIEC)");
	} else {
	  if (msgrcv(kolPisz, &komSynchro, 0, nrBloku, 0) == -1)
	    syserr("msgrcv (kolPisz)");
	  if (msgsnd(kolCat, &komBlok, dlugosc, 0) == -1)
	    syserr("msgsnd (kolCat)");
	  komSynchro.typ++;
	  if (msgsnd(kolPisz, &komSynchro, 0, 0) == -1)
	    syserr("msgsnd (kolPisz)");
	}
	break;
      default:
	break;
    }
  }
  
  close(plik);

  komSystem.typ = KS_ZGINALEM;
  if (msgsnd(kolSys, &komSystem, 0, 0) == -1)
    syserr("msgsnd (kolSys: ZGINALEM)");

  return 0;
}
