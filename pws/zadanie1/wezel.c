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
  printf("wezel (c) Maciek Makowski 2001\n\n");
  printf("\tSposob uzycia: wezel [lancuchZnakow]\n\n");
  exit(0);
}


int main(int argc, char **argv)
{
  char slowo[255];
  unsigned char polecenie, koniec;  
  unsigned char napis[4096], bufor[4096], dlugosc[2]; 
  int lacze[2][2] = {{brak, brak}, {brak, brak}}; /* identyfikatory wejsc i wyjsc do lewego i prawego syna */

  if (argc < 2)
    opisProgramu();

  strcpy(slowo, argv[1]);
  koniec = 0;
  while (!koniec) {
    read(0, &polecenie, 1);
    switch (polecenie) {
      case 'x': /* likwidacja drzewa                        */
        if (lacze[lewy][wyjscie] != brak)
          piszDrzewo(lacze[lewy][wyjscie], 'x');
        if (lacze[prawy][wyjscie] != brak)
          piszDrzewo(lacze[prawy][wyjscie], 'x');
        koniec = 1;
        break;

      case 'w': 
        /* wypisywanie                                      *
         *  wysyla na standardowe wyjscie:                  *
         *  2 bajty  :: dlugosc wypisywanego slowa (N       *
         *  N bajtow :: slowo do wypisania                  */
        strcpy(napis, "(");
        if (lacze[lewy][wyjscie] != brak) {
          piszDrzewo(lacze[lewy][wyjscie], 'w');
          read(lacze[lewy][wejscie], dlugosc, 2);
          read(lacze[lewy][wejscie], bufor, dekoduj2B(dlugosc));
          bufor[dekoduj2B(dlugosc)] = 0;
          strcat(napis, bufor);
          strcat(napis, " ");
        }
        strcat(napis, slowo);
        if (lacze[prawy][wyjscie] != brak) {
          strcat(napis, " ");
          piszDrzewo(lacze[prawy][wyjscie], 'w');
          read(lacze[prawy][wejscie], dlugosc, 2);
          read(lacze[prawy][wejscie], bufor, dekoduj2B(dlugosc));
          bufor[dekoduj2B(dlugosc)] = 0;
          strcat(napis, bufor);
        }
        strcat(napis, ")");
        koduj2B(dlugosc, strlen(napis));
        write(wyjscie, dlugosc, 2);
        write(wyjscie, napis, strlen(napis));
        break;

      case 'i':
        /* wstawianie nowego slowa                          *
         *  dostaje ze standardowego wejscia:               *
         *  2 bajty  :: dlugosc slowa do wstawienia (N)     *
         *  N bajtow :: slowo do wstawienia                 */
        read(wejscie, dlugosc, 2);
        read(wejscie, bufor, dekoduj2B(dlugosc));
        bufor[dekoduj2B(dlugosc)] = 0;
        if (strcmp(bufor, slowo) < 0) {
          if (lacze[lewy][wyjscie] != brak)
            wstawSlowo(lacze[lewy][wyjscie], bufor);
          else
            tworzWezel(bufor, lacze[lewy]);
        } else {
          if (lacze[prawy][wyjscie] != brak)
            wstawSlowo(lacze[prawy][wyjscie], bufor);
          else
            tworzWezel(bufor, lacze[prawy]);
        }
        break;

      default: /* diagnostyka                               */
        write(wyjscie, &polecenie, 1);
        break;
    }
  }

  return(0);
}


