#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "err.h"

#ifdef MK_WATKI
#include <pthread.h>
#include "suma_watki.h"
#endif
#ifdef MK_FIFO
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include "suma_synchr.h"
#include "suma_fifo.h"
#endif
#ifdef MK_IPC
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "suma_synchr.h"
#include "suma_ipc.h"
#endif


int wczytanieDanych(char ***liczba, int *ileLiczb, int *dlugoscLiczby, int *dlugoscKom)
{
  int i;

  scanf("%d, %d, %d\n", ileLiczb, dlugoscLiczby, dlugoscKom);
  if (*ileLiczb < 1) {
    fprintf(stderr, "Nieprawidlowa liczba danych wejsciowych (musi byc >= 1)\n");
    return 0;
  }
  if (*dlugoscLiczby < 2) {
    fprintf(stderr, "Nieprawidlowa dlugosc liczby (musi byc >= 2)\n");
    return 0;
  }
    
  if ((*liczba = calloc(*ileLiczb, sizeof(char*))) == 0)
    syserr("calloc");

  for (i = 0; i < *ileLiczb; i++) {
    if (((*liczba)[i] = malloc(*dlugoscLiczby + 1)) == 0)
      syserr("malloc");
    scanf("%s\n", (*liczba)[i]);
    if (strlen((*liczba)[i]) != *dlugoscLiczby) {
      fprintf(stderr, "Bledna dlugosc liczby wejsciowej (%d liczba z kolei)\n", i + 1);
      return 0;
    }
  }

  return *ileLiczb;
}


int main()
{
  char **liczby;
  int ileLiczb, dlugoscLiczby, ileProcesow, dlugoscKom;
  struct timeval tvStart, tvKoniec;
  long sec, usec;

  SP_DEKLARACJE_ZMIENNYCH
  SP_INICJACJA_ZMIENNYCH
  
  wczytanieDanych(&liczby, &ileLiczb, &dlugoscLiczby, &dlugoscKom);
  ileProcesow = dlugoscLiczby + (ileLiczb - 1) / 10 + 1;

  sp_uruchomienieProcesow(SP_URUCHOMIENIEPROCESOW_PARAM);
  if (gettimeofday(&tvStart, 0) != 0)
    syserr("gettimeofday");
  sp_startObliczen(SP_STARTOBLICZEN_PARAM);
  sp_czekanieNaKoniec(SP_CZEKANIENAKONIEC_PARAM);

  if (gettimeofday(&tvKoniec, 0) != 0)
    syserr("gettimeofday");
  sec = tvKoniec.tv_sec - tvStart.tv_sec;
  usec = tvKoniec.tv_usec - tvStart.tv_usec;
  if (usec < 0) {
    sec--;
    usec += 1000000;
  }
  printf("czas: %ld.%06ld s\n", sec, usec);

  sp_zebranieWynikow(SP_ZEBRANIEWYNIKOW_PARAM);

  return 0;
}

















