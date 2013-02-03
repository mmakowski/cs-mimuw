#include <stdio.h>
#include "suma_ascii.h"


void suma(char *zrodlo, char *cel)
{
  int i, p, t;

  i = strlen(zrodlo) - 1;
  p = 0;
  for (; i >= 0; i--) {
    t = zrodlo[i] + cel[i] - 96 + p;
    cel[i] = (char)(t % 10 + 48);
    p = (t >= 10) ? 1 : 0;
  }
}
