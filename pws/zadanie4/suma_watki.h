#ifndef suma_watki_h
#define suma_watki_h

#define SP_DEKLARACJE_ZMIENNYCH \
pthread_cond_t start = PTHREAD_COND_INITIALIZER;\
pthread_mutex_t glob = PTHREAD_MUTEX_INITIALIZER;\
char **wynik;\
sp_proces_t *procesy;

#define SP_INICJACJA_ZMIENNYCH

#define SP_URUCHOMIENIEPROCESOW_PARAM ileProcesow, &procesy, &wynik, dlugoscLiczby, ileLiczb, dlugoscKom, liczby, &glob, &start
#define SP_STARTOBLICZEN_PARAM &glob, &start
#define SP_CZEKANIENAKONIEC_PARAM ileProcesow, procesy
#define SP_ZEBRANIEWYNIKOW_PARAM ileProcesow, ileLiczb, wynik

#define sp_proces_t pthread_t

void sp_uruchomienieProcesow(int ile, sp_proces_t **procesy, char ***wynik, int dlugoscLiczby, int ileLiczb, int dlugoscKom, char **liczby, pthread_mutex_t *glob, pthread_cond_t *start);
void sp_startObliczen(pthread_mutex_t *glob, pthread_cond_t *start);
void sp_czekanieNaKoniec(int ile, sp_proces_t *procesy);
void sp_zebranieWynikow(int ileProcesow, int ileLiczb, char **wynik);

#endif
