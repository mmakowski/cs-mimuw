#ifndef SUMA_SYNCHR_H
#define SUMA_SYNCHR_H

#define KK_DANE_CYFRY 200 /* ile cyfr jest przekazywane w jednej porcji */
#define KK_DANE_SYS 1

#define KOL_KEY_SYS 1234L
#define KOL_KEY_CYFRY 1235L
#define SEM_KEY_START 1236L

#define KSYS_START 1L
#define KSYS_KONIEC 2L
#define KSYS_POLICZONE 3L
#define KSYS_SKONCZ 4L

typedef struct {
  long typ;
  char dane[KK_DANE_SYS];
} komSys;

typedef struct {
  long numer;
  char cyfry[KK_DANE_CYFRY];
} komCyfry;



#endif
