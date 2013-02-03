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

int main()
{
  struct ttcp_socket_t *sock;
  struct sockaddr_in myaddr, rmtaddr;
  socklen_t rmtaddrlen;
  char buf[BUF_SIZE];
  size_t size;
  int f, pos, n;
  struct stat fst;

  if ((sock = ttcp_socket()) == 0)
    DEBUG("error: ttcp_socket\n");
  
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = INADDR_ANY;
  myaddr.sin_port = htons(PORT);

  if (ttcp_bind(sock, (struct sockaddr *)&myaddr, sizeof myaddr) == TTCP_ERR)
    DEBUG("error: ttcp_bind\n");

  while (1) {
    if (ttcp_accept(sock, (struct sockaddr *)&rmtaddr, &rmtaddrlen) == TTCP_SUCC) {
      DEBUG("Client connected\n");
      if (ttcp_read(sock, &size, sizeof(size_t)) != sizeof(size_t))
	DEBUG("error: ttcp_read (filename length)\n");
      DEBUG1("file name length: %d\n", size);
      if (ttcp_read(sock, buf, size) != size)
	DEBUG("error: ttcp_read (filename)\n");
      buf[size] = 0;
      DEBUG1("file name: %s\n", buf);
      
      buf[size] = 0;
      if ((f = open(buf, O_RDONLY)) < 0)
	DEBUG("error: open\n");
      size = 0;
      if (fstat(f, &fst) != 0)
	DEBUG("error: fstat\n");
      else
	size = fst.st_size;

      if (ttcp_write(sock, &size, sizeof(size_t)) != sizeof(size_t))
	DEBUG("error: ttcp_write (file size)\n");
      
      if (size > 0) {
	pos = 0;
	while (pos < size) {
	  n = (size - pos >= BUF_SIZE) ? BUF_SIZE : size - pos;
	  if (read(f, buf, n) < n)
	    DEBUG("error: read\n");
	  if (ttcp_write(sock, buf, n) < n)
	    DEBUG("error: ttcp_write (file contents)\n");
	  pos += n;
	}
	close(f);
      }
    }
  }

  return 0;
}






