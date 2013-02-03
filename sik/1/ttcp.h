#ifndef TTCP_H
#define TTCP_H

#define TTCP_ERR -1
#define TTCP_SUCC 0

struct ttcp_socket_t;

/* interfejs */
extern struct ttcp_socket_t *ttcp_socket();
extern int ttcp_close(struct ttcp_socket_t *sock);
extern int ttcp_bind(struct ttcp_socket_t *sock, struct sockaddr *myaddr, socklen_t addrlen);
extern int ttcp_accept(struct ttcp_socket_t *sock, struct sockaddr *clntaddr, socklen_t *addrlen);
extern int ttcp_connect(struct ttcp_socket_t *sock, struct sockaddr *servaddr, socklen_t addrlen);
extern int ttcp_read(struct ttcp_socket_t *sock, void *buf, size_t count);
extern int ttcp_write(struct ttcp_socket_t *sock, const void *buf, size_t count);

#endif /* TTCP_H */
