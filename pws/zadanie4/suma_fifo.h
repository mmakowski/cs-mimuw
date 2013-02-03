#ifndef SUMA_FIFO_H
#define SUMA_FIFO_H

#define SP_DEKLARACJE_ZMIENNYCH \
int kolSys, kolCyfry; /* kolejki do komunikacji z procesami obliczeniowymi */\
int semStart;         /* semafor startujacy zawodnikow                     */ 

#define SP_INICJACJA_ZMIENNYCH \
if ((kolSys = msgget(KOL_KEY_SYS, 0600 | IPC_CREAT | IPC_EXCL)) == -1)\
syserr("msgget (kolSys)");\
if ((kolCyfry = msgget(KOL_KEY_CYFRY, 0600 | IPC_CREAT | IPC_EXCL)) == -1)\
syserr("msgget (kolCyfry)");\
if ((semStart = semget(SEM_KEY_START, 1, 0600 | IPC_CREAT | IPC_EXCL)) == -1)\
syserr("semget (semStart)");

#define SP_URUCHOMIENIEPROCESOW_PARAM ileProcesow, kolSys, semStart, dlugoscLiczby, ileLiczb, dlugoscKom, liczby
#define SP_STARTOBLICZEN_PARAM semStart
#define SP_CZEKANIENAKONIEC_PARAM ileProcesow, kolSys
#define SP_ZEBRANIEWYNIKOW_PARAM ileProcesow, ileLiczb, kolCyfry, kolSys

#define sp_proces_t pid_t

void sp_uruchomienieProcesow(int ile, int kolSys, int semStart, int dlugoscLiczby, int ileLiczb, int dlugoscKom, char **liczby);
void sp_startObliczen(int semStart);
void sp_czekanieNaKoniec(int ile, int kolSys);
void sp_zebranieWynikow(int ileProcesow, int ileLiczb, int kolCyfry, int kolSys);

#endif
