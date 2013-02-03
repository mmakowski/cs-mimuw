#ifndef _DRZEWO_H_
#define _DRZEWO_H_

#define lewy 0
#define prawy 1
#define wejscie 0
#define wyjscie 1
#define brak -1

#define koduj2B(x, y) (x)[0] = (y) / 256; (x)[1] = (y) % 256;
/* zapisuje w tablicy x kolejno starszy i mlodszy bajt *
 * liczby y                                            */

#define dekoduj2B(x) ((x)[0] * 256 + (x)[1])
/* zwraca liczbe zapisana w pierwszych dwoch komorkach *
 * tablicy x                                           */

void piszDrzewo(int wejscieDrzewa, char polecenie);
/* wypisuje ma wejscieDrzewa jeden bajt - polecenie    */ 

void wstawSlowo(int wejscieDrzewa, const char *slowo);
/* wstawia do drzewa identyfikowanego przez            *
 * wejscieDrzewa slowo na ktore wskazuje *slowo        */

void tworzWezel(const char *slowo, int *lacze);
/* tworzy nowy wezel zawierajacy slowo, komunikujacy   *
 * sie z ojcem za posrednictwem lacz *lacze            *
 *                                                     *
 * MODYFIKOWANE PARAMETRY:                             *
 *  lacze - lacze[wejscie] i lacze[wyjscie] zostaja    *
 *  ustawione odpowienio na zapis do lacza wejsciowego *
 *  i na odczyt z lacza wyjsciowego utworzonego wezla  */

#endif
