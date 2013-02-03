#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "ttcp.h"
#include "tftp.h"

#define DEBUG(s) fprintf(stderr, s)
#define DEBUG1(s, a1) fprintf(stderr, s, a1)

#define BUF_SIZE 1024

int main(int argc, char **argv)
{
  struct ttcp_socket_t *sock;
  struct sockaddr_in rmtaddr;
  struct hostent *hp;
  char buf[BUF_SIZE];
  int n, t, f;
  size_t size;

  if (argc < 3) {
    printf("usage: tftp_clnt <host> <file>\n");
    exit(1);
  }

  if ((sock = ttcp_socket()) == 0)
    DEBUG("error: ttcp_socket\n");
  
  if ((hp = gethostbyname(argv[1])) == 0)
    DEBUG("error: gethostbyname\n");

  rmtaddr.sin_family = AF_INET;
  memcpy(&rmtaddr.sin_addr, hp->h_addr, hp->h_length);
  rmtaddr.sin_port = htons(PORT);

  printf("Connecting to %s...\n", argv[1]);
  if (ttcp_connect(sock, (struct sockaddr *)&rmtaddr, sizeof rmtaddr) == TTCP_ERR) {
    DEBUG1("Could not connect to %s\n", argv[1]);
    return 1;
  }

  size = strlen(argv[2]);
  if (ttcp_write(sock, &size, sizeof(size_t)) < sizeof(size_t))
    DEBUG("error: ttcp_write (filename length)\n");

  if (ttcp_write(sock, argv[2], strlen(argv[2])) < strlen(argv[2]))
    DEBUG("error: ttcp_write (filename)\n");

  size = 0;
  if (ttcp_read(sock, &size, sizeof(size_t)) < sizeof(size_t))
    DEBUG("error: ttcp_read (file length))\n");

  if (size > 0) {
    f = open("download", O_WRONLY | O_CREAT);
    n = 0;
    while (n < size) {
      if ((t = ttcp_read(sock, buf, BUF_SIZE)) == TTCP_ERR)
	DEBUG("error: ttcp_read (file contents)\n");
      write(f, buf, t);
      n += t;
      printf("downloaded %d/%d bytes\r", n, size);
      fflush(stdout);
    }
    printf("\n");
    close(f);
  } else
    printf("%s doesn't exist\n", argv[2]);

  if (ttcp_close(sock) == TTCP_ERR)
    DEBUG("error: ttcp_close\n");

  return 0;
}
