#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "err.h"

/*#define DEBUG*/

#define A 0
#define B 1
#define WOLNE -1
#define KONIEC -2

#define MUTEX_LOCK(m) if ((blad = pthread_mutex_lock(m)) != 0) syserr("mutex_lock", blad)
#define MUTEX_UNLOCK(m) if ((blad = pthread_mutex_unlock(m)) != 0) syserr("mutex_unlock", blad)
#define COND_WAIT(c, m) if ((blad = pthread_cond_wait((c), (m))) != 0) syserr("cond_wait", blad)
#define COND_SIGNAL(c) if ((blad = pthread_cond_signal(c)) != 0) syserr("cond_signal", blad)
#define COND_BROADCAST(c) if ((blad = pthread_cond_broadcast(c)) != 0) syserr("cond_signal", blad)


struct infoInicjacji {
  char skopiowane;
  pthread_mutex_t ini;
  pthread_cond_t moznaZmieniac;
};

struct infoProcesu {
  char grupa;
  int numer;
  pthread_cond_t *moznaWalczyc;  
  pthread_cond_t *jestMiejsce;
  pthread_mutex_t *walka;        /* muteks zabezpieczajacy pojedynek    */
  pthread_mutex_t *miejsce;      /* muteks zabezpieczajacy licznik poj. */
  char *wartosc;
  char *koniec;
  int *wolne;
  int limit;
  struct infoInicjacji *infoIni;
};


void opisProgramu()
{
  printf("pojedynki (c) 2001 Maciek Makowski\n\n");
  printf("program symuluje pojedynki miedzy procesami.\n");
  printf("\tSposob uzycia: pojedynki n k\n");
  printf("n - liczba procesow w kazdej z grup (>0)\n");
  printf("k - maksymalna liczba pojedynkow ktore moga odbywac sie jednoczesnie (>0)\n\n");

  exit(0);
}


void inicjacjaInfoInicjacji(struct infoInicjacji *info)
{
  int blad;

  info->skopiowane = 0;
  if ((blad = pthread_mutex_init(&(info->ini), 0)) != 0)
    syserr("mutex_init (ini)", blad);
  if ((blad = pthread_cond_init(&(info->moznaZmieniac), 0)) != 0)
    syserr("cond_init (moznaZmieniac)", blad);
}


/* tresc watku (procesu)                                             *
 * dane - wskaznik do struktury infoProcesu                          */
void *proces(void *dane)
{
  int blad;
  int zwyciestwa = 0;
  struct infoProcesu info, *zrodlo;
  char wartosc;

  /* kopiowanie informacji watku                                     */
  zrodlo = (struct infoProcesu *)dane;
  MUTEX_LOCK(&(zrodlo->infoIni->ini));
  info.grupa = zrodlo->grupa;
  info.numer = zrodlo->numer;
  info.moznaWalczyc = zrodlo->moznaWalczyc;
  info.walka = zrodlo->walka;
  info.jestMiejsce = zrodlo->jestMiejsce;
  info.miejsce = zrodlo->miejsce;
  info.wartosc = zrodlo->wartosc;
  info.koniec = zrodlo->koniec;
  info.wolne = zrodlo->wolne;
  info.limit = zrodlo->limit;
  zrodlo->infoIni->skopiowane = 1;
  COND_SIGNAL(&(zrodlo->infoIni->moznaZmieniac));
  MUTEX_UNLOCK(&(zrodlo->infoIni->ini));

  /* glowna petla procesu                                            */
  MUTEX_LOCK(info.miejsce);
  while (*(info.koniec) == 0) {
    MUTEX_UNLOCK(info.miejsce);

    /* wlasne sprawy                                                 */
    sleep(rand() & 1);

    /* pojedynek                                                     */
    wartosc = rand() % 64;

    MUTEX_LOCK(info.walka);
    if (*(info.wartosc) == WOLNE) {
      *(info.wartosc) = wartosc;
      COND_WAIT(info.moznaWalczyc, info.walka);
      if (*(info.wartosc) != KONIEC) {
	if (*(info.wartosc) < wartosc)
	  zwyciestwa++;
	else if (*(info.wartosc) == wartosc && info.grupa == A)
	  zwyciestwa++;
	*(info.wartosc) = WOLNE;
	COND_SIGNAL(info.moznaWalczyc);
	MUTEX_UNLOCK(info.walka);
	MUTEX_LOCK(info.miejsce);
	++*(info.wolne);
	COND_SIGNAL(info.jestMiejsce);
	MUTEX_UNLOCK(info.miejsce);
      } else
	MUTEX_UNLOCK(info.walka);
    } else if (*(info.wartosc) == KONIEC) {
      MUTEX_UNLOCK(info.walka);
    } else {
      MUTEX_LOCK(info.miejsce);
      if (*(info.wolne) == 0)
	COND_WAIT(info.jestMiejsce, info.miejsce);
      --*(info.wolne);
      MUTEX_UNLOCK(info.miejsce);
      if (*(info.wartosc) < wartosc)
	zwyciestwa++;
      else if (*(info.wartosc) == wartosc && info.grupa == A)
	zwyciestwa++;
      *(info.wartosc) = wartosc;
      COND_SIGNAL(info.moznaWalczyc);
      COND_WAIT(info.moznaWalczyc, info.walka);
      MUTEX_UNLOCK(info.walka);
    }
 
    MUTEX_LOCK(info.miejsce);
    if (zwyciestwa == info.limit)
      *(info.koniec) = 1;

    if (*(info.koniec) == 1) {
      MUTEX_LOCK(info.walka);
      *(info.wartosc) = KONIEC;
      COND_SIGNAL(info.moznaWalczyc);
      MUTEX_UNLOCK(info.walka);
    }
  }

  printf("(%s, %d)\t: %d\n", (info.grupa == 0) ? "A": "B", info.numer, zwyciestwa);
  MUTEX_UNLOCK(info.miejsce);

  return (void *)0;
}


int main(int argc, char **argv)
{
  pthread_t *grupa[2];
  pthread_cond_t *moznaWalczyc, jestMiejsce;
  pthread_mutex_t *walka, miejsce;
  char *wartosc;
  pthread_attr_t atrybuty;
  int blad, ileProcesow, ileNaraz, i, wolne;
  char koniec;
  struct infoProcesu info;
  struct infoInicjacji infoIni;

  if (argc < 3)
    opisProgramu();
  if ((ileProcesow = atoi(argv[1])) < 1)
    opisProgramu();
  if ((ileNaraz = atoi(argv[2])) < 1)
    opisProgramu();

  wolne = ileNaraz;
  koniec = 0;

  if ((grupa[A] = calloc(ileProcesow, sizeof(pthread_t))) == 0)
    syserr("calloc (A)");
  if ((grupa[B] = calloc(ileProcesow, sizeof(pthread_t))) == 0)
    syserr("calloc (B)");
  if ((moznaWalczyc = calloc(ileProcesow, sizeof(pthread_cond_t))) == 0)
    syserr("calloc (cond)");
  if ((walka = calloc(ileProcesow, sizeof(pthread_mutex_t))) == 0)
    syserr("calloc (mutex)");
  for (i = 0; i < ileProcesow; i++) {
    if ((blad = pthread_mutex_init(&walka[i], 0)) != 0)
      syserr("mutex_init (walka)", blad);
    if ((blad = pthread_cond_init(&moznaWalczyc[i], 0)) != 0)
      syserr("cond_init (walka)", blad);
  }

  if ((wartosc = malloc(ileProcesow)) == 0)
    syserr("malloc (wartosc)");
  for (i = 0; i < ileProcesow; i++)
    wartosc[i] = WOLNE;

  if ((blad = pthread_cond_init(&jestMiejsce, 0)) != 0)
    syserr("cond_init (jestMiejsce)", blad);
  if ((blad = pthread_mutex_init(&miejsce, 0)) != 0)
    syserr("mutex_init (miejsce)", blad);
  if ((blad = pthread_attr_init(&atrybuty)) != 0)
    syserr("attr_init", blad);
  if ((blad = pthread_attr_setdetachstate(&atrybuty, PTHREAD_CREATE_JOINABLE)) != 0)
    syserr("attr_setdetachstate", blad);
  
  inicjacjaInfoInicjacji(&infoIni);
  info.infoIni = &infoIni;
  info.koniec = &koniec;
  info.jestMiejsce = &jestMiejsce;
  info.miejsce = &miejsce;
  info.wolne = &wolne;
  info.limit = ileProcesow;

  MUTEX_LOCK(&miejsce);
  MUTEX_LOCK(&(infoIni.ini));
  for (i = 1; i <= ileProcesow; i++) {
    info.numer = i;
    info.moznaWalczyc = &moznaWalczyc[i - 1];
    info.walka = &walka[i - 1];
    info.wartosc = &wartosc[i - 1];
    info.grupa = A;
    infoIni.skopiowane = 0;
    MUTEX_UNLOCK(&(infoIni.ini));

    if ((blad = pthread_create(&grupa[A][i - 1], &atrybuty, proces, (void *)&info)) != 0)
      syserr("create (A)", blad);

    /* oczekiwanie na skopiowanie danych                             */    
    MUTEX_LOCK(&(infoIni.ini));
    if (infoIni.skopiowane == 0) 
      COND_WAIT(&(infoIni.moznaZmieniac), &(infoIni.ini));
    info.grupa = B;
    infoIni.skopiowane = 0;
    MUTEX_UNLOCK(&(infoIni.ini));

    if ((blad = pthread_create(&grupa[B][i - 1], &atrybuty, proces, (void *)&info)) != 0)
      syserr("create (B)", blad);

    /* oczekiwanie na skopiowanie danych                             */
    MUTEX_LOCK(&(infoIni.ini));
    if (infoIni.skopiowane == 0) 
      COND_WAIT(&(infoIni.moznaZmieniac), &(infoIni.ini));
  }
  MUTEX_UNLOCK(&(infoIni.ini));
  MUTEX_UNLOCK(&miejsce);

  for (i = 1; i <= ileProcesow; i++) {
    if ((blad = pthread_join(grupa[A][i - 1], 0)) != 0)
      syserr("join (A)", blad);
    if ((blad = pthread_join(grupa[B][i - 1], 0)) != 0)
      syserr("join (B)", blad);
  }

  if ((blad = pthread_attr_destroy(&atrybuty)) != 0)
    syserr("attr_destroy", blad);
  for (i = 0; i < ileProcesow; i++) {
    if ((blad = pthread_mutex_destroy(&walka[i])) != 0)
      syserr("mutex_destroy (walka)", blad);
    if ((blad = pthread_cond_destroy(&moznaWalczyc[i])) != 0)
      syserr("cond_destroy (moznaWalczyc)", blad);
  }

  if ((blad = pthread_mutex_destroy(&miejsce)) != 0)
    syserr("mutex_destroy (miejsce)", blad);
  if ((blad = pthread_cond_destroy(&jestMiejsce)) != 0)
    syserr("cond_destroy (jestMiejsce)", blad);

  return 0;
}










