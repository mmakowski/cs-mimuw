#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "err.h"
#include "drzewo.h"


static void opisProgramu()
{
  printf("bst (c) Maciek Makowski 2001\n\n");
  printf("program generuje BST zawierajace podana liczbe slow Fibonacciego.\n\n");
  printf("\tSposob uzycia: bst [N]\n\n");
  printf("N - liczba z przedzialu [1, 13] oznaczajaca liczbe\n");
  printf("    slow Fibonacciego do wstawienia\n\n");
  exit(0);
}


static char *fibonacci(int n)
/* generuje n-te slowo Fibonacciego           */ 
{
  char *l, *p, *slowo = malloc(234);

  if (slowo == 0)
    syserr("malloc");

  switch (n) {
    case 1:
      slowo[0] = 'a';
      slowo[1] = 0;
      break;
    case 2:
      slowo[0] = 'b';
      slowo[1] = 0;
      break;
    default:
      l = fibonacci(n - 2);
      p = fibonacci(n - 1);
      strcat(strcpy(slowo, l), p);
      free(l);
      free(p);
      break;
  }
  return slowo;
}


int main(int argc, char **argv)
{
  int i, n, lacze[2];
  unsigned char bufor[4096], *slowo;

  if (argc < 2)
    opisProgramu();

  n = atoi(argv[1]);
  if (n < 1 || n > 13)
    opisProgramu();
  
  slowo = fibonacci(1);
  tworzWezel(slowo, lacze);
  free(slowo);

  for(i = 2; i <= n; i++) {
    slowo = fibonacci(i);
    wstawSlowo(lacze[wyjscie], slowo);
    free(slowo);
  }
 
  piszDrzewo(lacze[wyjscie], 'w');
  read(lacze[wejscie], bufor, 2);  /* odczytanie dlugosci napisu */
  i = dekoduj2B(bufor);
  read(lacze[wejscie], bufor, i);
  bufor[i] = 0;
  write(0, bufor, strlen(bufor));
  write(0, "\n", 1);
      
  piszDrzewo(lacze[wyjscie], 'x');

  if(wait(0) == -1)
    syserr("Error in wait\n");
  exit(0);
}












