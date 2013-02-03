/*
 * rozwiazanie zadania 2 z SIK LAB (RPC)
 * autor: Maciek Makowski (189188)
 *
 * serwer
 */


#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "prime.h"
#include "prime_xdr.h"


extern int registerrpc(unsigned long, unsigned long, unsigned long, 
		       char*, xdrproc_t, xdrproc_t);

extern char *zglos_sie_svc(char *);
extern char *sprawdz_liczbe_svc(char *);
extern char *koniec_sesji_svc(char *);
extern char *ustaw_dzielnik_svc(char *);

int main(int argc, char **argv)
{
  pmap_unset(PRIME_PROG, PRIME_VERS);
  registerrpc(PRIME_PROG, PRIME_VERS, ZGLOS_SIE, 
	      (char *)zglos_sie_svc, (xdrproc_t)xdr_str, (xdrproc_t)xdr_int);
  registerrpc(PRIME_PROG, PRIME_VERS, USTAW_DZIELNIK, 
	      (char *)ustaw_dzielnik_svc, (xdrproc_t)xdr_int, (xdrproc_t)xdr_void);
  registerrpc(PRIME_PROG, PRIME_VERS, SPRAWDZ_LICZBE, 
	      (char *)sprawdz_liczbe_svc, (xdrproc_t)xdr_check_req, (xdrproc_t)xdr_int);
  registerrpc(PRIME_PROG, PRIME_VERS, KONIEC_SESJI, 
	      (char *)koniec_sesji_svc, (xdrproc_t)xdr_str, (xdrproc_t)xdr_void);

  svc_run();

  exit(1);
}




