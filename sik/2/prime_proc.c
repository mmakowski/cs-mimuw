/*
 * implementacja procedur udostepnianych przez serwer
 */

#include <rpc/rpc.h>
#include <stdlib.h>
#include <string.h>
#include "prime.h"

#undef _DEBUG

#ifdef _DEBUG
#define DEBUG(s) fprintf(stderr, s)
#define DEBUG1(s, p) fprintf(stderr, s, p)
#define DEBUG2(s, p1, p2) fprintf(stderr, s, p1, p2)
#else
#define DEBUG(s)
#define DEBUG1(s, p)
#define DEBUG2(s, p1, p2)
#endif



static int liczba = -1;
static char sesja = 0;
static char *host;
static int max_spr = 0;


/*
 * ustawianie dzielnika
 */
char *ustaw_dzielnik_svc(char *l)
{
  liczba = *((int *)l);
  DEBUG1("SERVER: divider set to %d\n", *((int *)l));

  return 0;
}


/*
 * zglaszanie gotowosci
 */
char *zglos_sie_svc(char *a)
{
  static int r;
  
  if (!sesja) {
    DEBUG1("SERVER: starting session (%s)\n", a);
    if ((host = malloc(strlen(a) + 1)) == 0)
      exit(1);
    strcpy(host, a);
    sesja = 1;
    r = 1;
  } else
    r = 0;

  return (char *)&r;
}


/*
 * sprawdzanie podzielnosci
 */
char *sprawdz_liczbe_svc(char *l)
{
  static int r;
  char *sess = l + sizeof(int);
  int li = *((int *)l);

  DEBUG1("SERVER: request from host %s\n", sess);
  DEBUG2("SERVER: checking %d mod %d\n", li, liczba);
  if (!sesja || liczba == -1 || li <= max_spr)
    r = -1;
  else if (strcmp(sess, host) != 0)
    r = -1;
  else {
    max_spr = li;
    if (li % liczba == 0) 
      r = 0;
    else
      r = 1;
  }

  return (char *)&r;
}


/*
 * zamykanie sesji
 */
char *koniec_sesji_svc(char *a)
{
  if (strcmp(a, host) == 0) {
    DEBUG1("SERVER: closing session (%s)\n", host);
    free(host);
    liczba = -1;
    sesja = 0;
    max_spr = 0;
  }

  return 0;
}







