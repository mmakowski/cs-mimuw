/*
 * rozwiazanie zadania 2 z SIK LAB (RPC)
 * autor: Maciek Makowski (189188)
 *
 * program klienta
 */


#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "prime.h"
#include "prime_xdr.h"


#undef _DEBUG

#ifdef _DEBUG
#define DEBUG(s) fprintf(stderr, s)
#define DEBUG1(s, p) fprintf(stderr, s, p)
#define DEBUG2(s, p1, p2) fprintf(stderr, s, p1, p2)
#else
#define DEBUG(s)
#define DEBUG1(s, p)
#define DEBUG2(s, p1, p2)
#endif


#define MAX_SERVERS 100

int count;
char res;
char my_name[MAX_STR_LEN];
char *servers[MAX_SERVERS];
int relevant;
int received;

/*
 * procedura wywolywana przez zglaszajacy sie serwer
 */
int server_active(int *ans, struct sockaddr_in *raddr)
{
  struct hostent *h;
  char *hostname, *hcpy;

  if (*ans) {
    h = gethostbyaddr((char *)&(raddr->sin_addr),
		      sizeof(struct in_addr),raddr->sin_family);
    if (h) 
      hostname = h->h_name;
    else 
      hostname = inet_ntoa(raddr->sin_addr);

    if ((hcpy = malloc(strlen(hostname) + 1)) == 0)
      exit(1);
    strcpy(hcpy, hostname);
    if (count < MAX_SERVERS)
      servers[count] = hcpy;
    
    count++;
    DEBUG1("CLNT: host: %s\n", hostname);
  }

  return 0;
}


/*
 * procedura wywolywana przy SPRAWDZ_LICZBE
 */
int server_sprawdzil(int *ans, struct sockaddr_in *raddr)
{
  struct hostent *h;
  char *hostname;

  h = gethostbyaddr((char *)&(raddr->sin_addr),
		    sizeof(struct in_addr),raddr->sin_family);
  if (h) 
    hostname = h->h_name;
  else 
    hostname = inet_ntoa(raddr->sin_addr);
    
  DEBUG2("CLNT: response from %s: %d\n", hostname, *ans);

  if (*ans == 0) {
    res = 0;
    //return 1;
    received++;
  } else if (*ans == 1)
    received++;

  if (received == relevant) 
    return 1;

  return 0;
}

/*
 * procedura wywolywana przez zamykany serwer
 */
int server_closing(int *ans, struct sockaddr_in *raddr)
{
  DEBUG("CLNT: host closed session\n");

  if (--count == 0)
    return 1;
   
  return 0;
}


/*
 * znajdowanie oczekujacych serwerow
 */
int find_servers()
{
  static int ans = 0;
  enum clnt_stat clnt_stat;

  count = 0;
  clnt_stat = clnt_broadcast(PRIME_PROG, PRIME_VERS,
			     ZGLOS_SIE, (xdrproc_t) xdr_str, my_name, (xdrproc_t) xdr_int,
			     (char *)&ans, (resultproc_t)server_active);

  return count;
}


/*
 * zamykanie sesji
 */
void close_session()
{
  enum clnt_stat clnt_stat;

  clnt_stat = clnt_broadcast(PRIME_PROG, PRIME_VERS,
			     KONIEC_SESJI, (xdrproc_t) xdr_str, my_name, (xdrproc_t) xdr_void,
			     0, (resultproc_t)server_closing);

}


/*
 * sprawdzanie pierwszosci
 */
char is_prime(int i)
{
  static int ans = -1;
  char chrq[sizeof(int) + MAX_STR_LEN];

  received = 0;
  res = 1;

  memcpy(chrq, &i, sizeof(int));
  memcpy(chrq + sizeof(int), my_name, MAX_STR_LEN);

  clnt_broadcast(PRIME_PROG, PRIME_VERS,
		 SPRAWDZ_LICZBE, (xdrproc_t) xdr_check_req, chrq, (xdrproc_t) xdr_int,
		 (char *)&ans, (resultproc_t)server_sprawdzil);

  return res;
}


/*
 * ustawianie dzielnika pierwszego wolnego serwera
 */
void set_divider(int d)
{
  DEBUG2("CLNT: setting divider for host %s to %d\n", servers[relevant], d);
  if (callrpc(servers[relevant], PRIME_PROG, PRIME_VERS, USTAW_DZIELNIK, 
	      (xdrproc_t) xdr_int, (char *)&d, (xdrproc_t) xdr_void, 0) != 0)
    exit(1);
  relevant++;
}


int main()
{
  int i, max;
  char first_after = 1;

  gethostname(my_name, MAX_STR_LEN - 1);
  DEBUG1("CLNT: name of this host: %s\n", my_name);

  DEBUG("CLNT: looking for servers...\n");
  find_servers();
  DEBUG1("CLNT: found %d servers\n", count);
  if (count == 0)
    return 0;
  relevant = 0;
  set_divider(2);
  printf("%6d\n", 2);

  max = (count + 1) * (count + 1) - 1;
  for (i = 3; i <= max; i++) {
    DEBUG1("CLNT: checking %d...\n", i);
    if (is_prime(i)) {
      if (relevant < count) {
	set_divider(i);
	max = (i + 1) * (i + 1) - 1;
      } else if (first_after) {
	first_after = 0;
	max = i * i - 1;
      }	
      printf("%6d\n", i);
    }
  }

  close_session();
  DEBUG("CLNT: session closed\n");

  return 0;
}








