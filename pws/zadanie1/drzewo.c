#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "err.h"
#include "drzewo.h"

void piszDrzewo(int wejscieDrzewa, char polecenie)
{
  write(wejscieDrzewa, &polecenie, 1);
}


void wstawSlowo(int wejscieDrzewa, const char *slowo)
{
  unsigned char dlugosc[2];
  piszDrzewo(wejscieDrzewa, 'i');
  koduj2B(dlugosc, strlen(slowo));
  write(wejscieDrzewa, dlugosc, 2);
  write(wejscieDrzewa, slowo, strlen(slowo));
}


void tworzWezel(const char *slowo, int *lacze)
{
  pid_t pid;
  int wDol[2], wGore[2];

  if (pipe(wDol) == -1)
    syserr("Error in pipe\n");

  if (pipe(wGore) == -1)
    syserr("Error in pipe\n");

  switch (pid = fork()) {
    case -1:
      syserr("Error in fork\n");
      
    case 0:
      if (close(0) == -1) syserr("child: close(0)\n");
      if (dup(wDol[0]) != 0) syserr("child: dup(wDol[0])\n");
      if (close(wDol[0]) == -1) syserr("child: close(wDol[0])\n");
      if (close(wDol[1]) == -1) syserr("child: close(wDol[1])\n");
      if (close(1) == -1) syserr("child: close(1)\n");
      if (dup(wGore[1]) != 1) syserr("child: dup(wGore[1])\n");
      if (close(wGore[0]) == -1) syserr("child: close(wGore[0])\n");
      if (close(wGore[1]) == -1) syserr("child: close(wGore[1])\n");
      execlp("./wezel", "wezel", slowo, 0); 
      syserr("Error in exec\n");
    default:
      if (close(wDol[0]) == -1) syserr("parent: close(wDol[0])\n");
      if (close(wGore[1]) == -1) syserr("parent: close(wGore[1])\n");      
      lacze[wyjscie] = wDol[1];
      lacze[wejscie] = wGore[0];
      break;
  }
}
