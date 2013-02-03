#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "err.h"

#include "suma_watki.h"
#include "suma_ascii.h"

#define KOM_ZMIENNE char *kom;
#define KOM_INI \
if (dp->dlKom > 0) \
  MALLOC(kom, dp->dlKom);

#ifdef MK_KOPIUJ
#define KOM_KOPIUJ \
if (dp->dlKom > 0) { \
  for (j = 0; j < dp->dlKom; j++) \
    kom[j] = dp->prawy->komunikat[i][j]; \
}
#endif

#ifndef MK_KOPIUJ
#define KOM_KOPIUJ
#endif

#define PR_NIEWYSLANE 0x01
#define PR_WYSLANE 0
#define PR_NOWE 0x02
#define PR_WYSLIJTERAZ (PR_NOWE + PR_NIEWYSLANE)

#define PR_NIEMA 0
#define PR_JEST 1
/* oznacza brak informacji o przeniesieniu                            */
#define KOM_BRAK 0

#define TRACE(s) //fprintf(stderr, "%s\n", (s))

#define CREATE(p, a, d) if ((blad = pthread_create((p), (a), proces, (void *)(d))) != 0) syserr("create", blad)
#define ATTR_INIT(a) if ((blad = pthread_attr_init(a)) != 0) syserr("attr_init", blad)
#define ATTR_SETJOINABLE(a) if ((blad = pthread_attr_setdetachstate((a), PTHREAD_CREATE_JOINABLE)) != 0) syserr("attr_setdetachstate", blad)
#define COND_BROADCAST(c) if ((blad = pthread_cond_broadcast(c)) != 0) syserr("cond_broadcast", blad)
#define COND_SIGNAL(c) if ((blad = pthread_cond_signal(c)) != 0) syserr("cond_signal", blad)
#define COND_WAIT(c, m) if ((blad = pthread_cond_wait((c), (m))) != 0) syserr("cond_wait", blad)
#define MUTEX_LOCK(m) if ((blad = pthread_mutex_lock(m)) != 0) syserr("mutex_lock", blad)
#define MUTEX_UNLOCK(m) if ((blad = pthread_mutex_unlock(m)) != 0) syserr("mutex_unlock", blad)
#define JOIN(t) if ((blad = pthread_join((t), 0)) != 0) syserr("join", blad)
#define MUTEX_INIT(m) if ((blad = pthread_mutex_init((m), 0)) != 0) syserr("mutex_init", blad)
#define COND_INIT(c) if ((blad = pthread_cond_init((c), 0)) != 0) syserr("cond_init", blad)

#define MALLOC(d, s) if (((d) = malloc(s)) == 0) syserr("malloc")
#define CALLOC(d, c, s) if (((d) = calloc((c), (s))) == 0) syserr("calloc")

struct daneKom {
  char **bufor;
  pthread_cond_t *saNowe;
  char nowe;
  char **komunikat;
  pthread_mutex_t *zajete;
};

struct daneProcesu {
  int ileLiczb;
  char *cyfry;
  int maxPrzen;
  int dlKom;
  struct daneKom *lewy;
  struct daneKom *prawy;
  pthread_mutex_t *glob;
  pthread_cond_t *start;
  pthread_cond_t *gotowy;
  char got;
};


struct daneKom *tworzDaneKom(int ileLiczb)
{
  struct daneKom *dk;
  int blad;
  int i;
  
  MALLOC(dk, sizeof(struct daneKom));
  dk->nowe = PR_NIEMA;
  CALLOC(dk->bufor, ileLiczb, sizeof(char *));
  CALLOC(dk->komunikat, ileLiczb, sizeof(char *));
  for (i = 0; i < ileLiczb - 1; i++) {
    dk->bufor[i] = PR_NIEMA;
    dk->komunikat[i] = PR_NIEMA;
  }
  MALLOC(dk->zajete, sizeof(pthread_mutex_t));
  MALLOC(dk->saNowe, sizeof(pthread_cond_t));

  MUTEX_INIT(dk->zajete);
  COND_INIT(dk->saNowe);

  return dk;
}

void *proces(void *dane)
{
  struct daneProcesu *dp;
  int blad, i, j, ilePrz = 0;
  char **bufor;
  char *doWyslania;
  KOM_ZMIENNE

  dp = (struct daneProcesu *)dane;

  CALLOC(bufor, dp->ileLiczb, sizeof(char *));
  MALLOC(doWyslania, dp->ileLiczb); 
  for (i = 0; i < dp->ileLiczb; i++) {
    MALLOC(bufor[i], dp->maxPrzen + 1);
    for (j = 0; j < dp->maxPrzen - 1; j++)
      bufor[i][j] = '0';
    bufor[i][dp->maxPrzen - 1] = dp->cyfry[i];
    bufor[i][dp->maxPrzen] = '\0';
    doWyslania[i] = PR_NIEWYSLANE;
  }

  KOM_INI

  MUTEX_LOCK(dp->glob);
  dp->got = 1;
  COND_SIGNAL(dp->gotowy);
  COND_WAIT(dp->start, dp->glob);
  TRACE("wystartowal");
  MUTEX_UNLOCK(dp->glob);
 
  for (i = 0; i < dp->ileLiczb - 1; i++)
    suma(bufor[i], bufor[i + 1]);

  if (dp->prawy != KOM_BRAK) {
    while (ilePrz < dp->ileLiczb) {
      MUTEX_LOCK(dp->prawy->zajete);
      if (dp->prawy->nowe != PR_JEST){
        TRACE("czeka na przeniesienie");
        COND_WAIT(dp->prawy->saNowe, dp->prawy->zajete);}
      TRACE("odbiera przeniesienie");
      dp->prawy->nowe = PR_NIEMA;
      for (i = 0; i < dp->ileLiczb; i++)
        if (dp->prawy->bufor[i] != PR_NIEMA) {
          suma(dp->prawy->bufor[i], bufor[i]);
          dp->prawy->bufor[i] = PR_NIEMA;
          doWyslania[i] += PR_NOWE;
          ilePrz++;
          KOM_KOPIUJ
        }
      MUTEX_UNLOCK(dp->prawy->zajete);
      if (dp->lewy != KOM_BRAK) {
        for (i = 0; i < dp->ileLiczb; i++)
          if (doWyslania[i] == PR_WYSLIJTERAZ) {
            MUTEX_LOCK(dp->lewy->zajete);
            MALLOC(dp->lewy->bufor[i], dp->maxPrzen + 1);
            dp->lewy->bufor[i][0] = '0';
            for (j = 0; j < dp->maxPrzen; j++)
              dp->lewy->bufor[i][j + 1] = bufor[i][j];
            dp->lewy->bufor[i][dp->maxPrzen] = '\0';
            dp->lewy->nowe = PR_JEST;
            if (dp->dlKom > 0)
              MALLOC(dp->lewy->komunikat[i], dp->dlKom);
            TRACE("wyslal przeniesienie");
            COND_SIGNAL(dp->lewy->saNowe);
            MUTEX_UNLOCK(dp->lewy->zajete);
            doWyslania[i] = PR_WYSLANE;
          }
      }
    }
  } else {
    TRACE("jest (1)");
    for (i = 0; i < dp->ileLiczb; i++) {
      MUTEX_LOCK(dp->lewy->zajete);
      MALLOC(dp->lewy->bufor[i], dp->maxPrzen + 1);
      dp->lewy->bufor[i][0] = '0';
      for (j = 0; j < dp->maxPrzen; j++)
        dp->lewy->bufor[i][j + 1] = bufor[i][j];
      dp->lewy->bufor[i][dp->maxPrzen] = '\0';
      dp->lewy->nowe = PR_JEST;
      if (dp->dlKom > 0)
        MALLOC(dp->lewy->komunikat[i], dp->dlKom);
      TRACE("wyslal przeniesienie (1)");
      COND_SIGNAL(dp->lewy->saNowe);
      MUTEX_UNLOCK(dp->lewy->zajete);
    }
  }
   
  for (i = 0; i < dp->ileLiczb; i++)
    dp->cyfry[i] = bufor[i][dp->maxPrzen - 1];

  TRACE("skonczyl");
  return 0;
}


void sp_uruchomienieProcesow(int ile, 
                             sp_proces_t **procesy,
                             char ***wynik,
                             int dlugoscLiczby,
                             int ileLiczb,
                             int dlugoscKom,
                             char **liczby,
                             pthread_mutex_t *glob,
                             pthread_cond_t *start)
{
  int i, j;
  int blad;
  pthread_cond_t gotowy = PTHREAD_COND_INITIALIZER;
  pthread_attr_t atrybuty;
  struct daneProcesu *dp;
  struct daneKom *dkl = KOM_BRAK, *dkp = KOM_BRAK;

  CALLOC(*procesy, ile, sizeof(sp_proces_t));
  CALLOC(*wynik, ile, sizeof(char *));

  ATTR_INIT(&atrybuty);
  ATTR_SETJOINABLE(&atrybuty);

  for (i = 0; i < ile; i++) {
    dkp = dkl;
    if (i < ile - 1)
      dkl = tworzDaneKom(ileLiczb);
    else
      dkl = KOM_BRAK;
    MALLOC(dp, sizeof(struct daneProcesu));
    MALLOC((*wynik)[i], ileLiczb);
    if (i < dlugoscLiczby)
      for (j = 0; j < ileLiczb; j++)
        (*wynik)[i][j] = liczby[j][dlugoscLiczby - i - 1];
    else
      for (j = 0; j < ileLiczb; j++)
        (*wynik)[i][j] = '0';
      
    dp->cyfry = (*wynik)[i];
    dp->glob = glob;
    dp->start = start;
    dp->ileLiczb = ileLiczb;
    dp->dlKom = dlugoscKom;
    dp->maxPrzen = ile - dlugoscLiczby + 1;
    dp->gotowy = &gotowy;
    dp->got = 0;
    dp->lewy = dkl;
    dp->prawy = dkp;
    CREATE(&(*procesy)[i], &atrybuty, (void *)dp);
    /* oczekiwanie na zakonczenie inicjalizacji w procesie           */
    MUTEX_LOCK(glob);
    if (!(dp->got))
      COND_WAIT(&gotowy, glob);
    MUTEX_UNLOCK(glob);
  }
}


void sp_startObliczen(pthread_mutex_t *glob, pthread_cond_t *start)
{
  int blad;
    
  MUTEX_LOCK(glob);
  TRACE("wysyla broadcast");
  COND_BROADCAST(start);
  MUTEX_UNLOCK(glob);
}

void sp_czekanieNaKoniec(int ile, sp_proces_t *procesy)
{
  int i;
  int blad;

  for (i = 0; i < ile; i++)
    JOIN(procesy[i]);
}

void sp_zebranieWynikow(int ileProcesow, int ileLiczb, char **wynik)
{
  int i, j;

#ifndef BEZ_WYNIKOW
  for (i = 0; i < ileLiczb; i++) {
    for (j = 0; j < ileProcesow; j++)
      printf("%c", wynik[ileProcesow - j - 1][i]);
    printf("\n");
  }
#endif
}









