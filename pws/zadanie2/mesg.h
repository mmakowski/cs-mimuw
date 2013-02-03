#ifndef mesg_h
#define mesg_h

#define	ROZMIAR_BLOKU 1024
/* rozmiar bloku odczytywanego przez agenta                 */

#define MAX_SYS_DANE 10
/* maksymalna liczba cyfr w numerze bloku                   */

typedef struct {
  long	typ;	
  char	dane[ROZMIAR_BLOKU];
} komCat;
/* komunikat do przesylania zawartosci pliku                */

#define KC_BLOK   1L
/* typy komunikatow w kolejce blokow                        */

typedef struct {
  long	typ;	
  char	dane[MAX_SYS_DANE];
} komSys;
/* komunikat do przesylania informacji "systemowych"        */

#define KS_ODBIERZ   2L
#define KS_GIN       3L
#define KS_KONIEC    4L
#define KS_ZGINALEM  6L
#define KS_DAJ_LIMIT 7L
#define KS_LIMIT     8L
/* typy komunikatow w kolejce "systemowej"                  */

typedef struct {
  long	typ;	
  char	dane[1];                      /* nieuzywane         */
} komSynch;
/* komunikat do synchronizacji odczytu i zapisu             */

#define	KOL_SYS	  1234L  /* kolejka "systemowa"             */
#define	KOL_CAT	  1235L  /* kolejka do przesylania pliku    */
#define	KOL_CZYT  1236L  /* kolejka do synchron. odczytu    */
#define	KOL_PISZ  1237L  /* kolejka do synchron. zapisu     */

#define SYS 0
#define CAT 1
#define CZYT 2
#define PISZ 3

#endif
