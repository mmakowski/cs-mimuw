/*
 * ttcp.c - biblioteka obslugujaca protokol TTCP
 * Maciek Makowski (189188)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include "ttcp.h"

#define DEBUG(s)  /* fprintf(stderr, s) */
#define DEBUG1(s, a1)  /* fprintf(stderr, s, a1) */


#define MSG_SIZE 1024
#define CONNECT_ATTEMPTS 5 
#define RETRANSMIT_ATTEMPTS 3
#define REPLY_TIMEOUT 5
#define MAX_LATENCY 15
#define QUEUE_SIZE 100
#define HEADER_SIZE sizeof(struct ttcp_msg_t)
#define DATA_SIZE MSG_SIZE - HEADER_SIZE
#define BUF_SIZE QUEUE_SIZE * DATA_SIZE

#define TTCP_MSG_NONE 0x00
#define TTCP_MSG_CONNECT 0x01
#define TTCP_MSG_ACCEPT 0x02
#define TTCP_MSG_REFUSE 0x04
#define TTCP_MSG_DATA 0x08
#define TTCP_MSG_CONFIRM 0x10
#define TTCP_MSG_ALIVE 0x20
#define TTCP_MSG_FIN 0x40


struct ttcp_msg_t {
  unsigned char type;
  unsigned int seqnum;    
  unsigned int acknum;    
  unsigned int window;  
  unsigned int datalen; 
};

struct ttcp_msgqueue_t {
  struct ttcp_msg_t *header;
  char *data;
  char age;
  char retrans;
  struct ttcp_msgqueue_t *prev, *next;
};

#define TRANSCEIVER_CMD_NONE    0x00
#define TRANSCEIVER_CMD_STOP    0x01
#define TRANSCEIVER_CMD_ACCEPT  0x02
#define TRANSCEIVER_CMD_CONNECT 0x04
#define TRANSCEIVER_CMD_FIN     0x08
#define TRANSCEIVER_CMD_FIN_NOASK      0x18

struct ttcp_socket_t {
  int fd;
  int window;
  char cmd;
  char connected;
  char buf[BUF_SIZE];
  char *buf_data_start;
  size_t buf_data_len;
  unsigned int sendseq, recvseq;
  struct ttcp_msgqueue_t sendq, ackq, recvq;
  unsigned int sendc, ackc, recvc;
  unsigned int unack_size;
  struct sockaddr *rmtaddr;
  time_t last_alive;
  socklen_t addrlen;
  pthread_cond_t buf_is_data, sendq_not_full, is_connected, do_connect;
  pthread_mutex_t mutex;
  pthread_t transceiver;
};
  
#define MSGQUEUE_INSERT(q, m) \
  (m)->next = (q).next;\
  (m)->prev = &(q);\
  (q).next = (m);\
  (m)->next->prev = (m);

#define MSGQUEUE_REMOVE(m) \
  (m)->prev->next = (m)->next;\
  (m)->next->prev = (m)->prev;

#define MUTEX_LOCK(m) if (pthread_mutex_lock(m) != 0) { perror("pthread_mutex_lock"); exit(1); }
#define MUTEX_UNLOCK(m) if (pthread_mutex_unlock(m) !=0 ) { perror("pthread_mutex_unlock"); exit(1); }
#define SETCMD(s, c) \
MUTEX_LOCK(&((s)->mutex));\
(s)->cmd = (c);\
MUTEX_UNLOCK(&((s)->mutex))


/*
 * wysylanie pakietu z danymi
 */
static int ttcp_send_msg(struct ttcp_socket_t *sock, struct ttcp_msgqueue_t *cm)
{
  char buf[MSG_SIZE];
  struct ttcp_msg_t *msg = cm->header;

  memcpy(buf, msg, HEADER_SIZE);
  memcpy(buf + HEADER_SIZE, cm->data, msg->datalen);
  if (send(sock->fd, buf, HEADER_SIZE + msg->datalen, 0) == -1)
    return TTCP_ERR;

  return TTCP_SUCC;
}


/*
 * wysylanie pakietow
 *
 * przegladana jest kolejka sendq i dopoki sa w niej pakiety i jest 
 * wystarczajaca ilosc miejsca w oknie, pakiety sa wysylane i przenoszone
 * do kolejki ackq
 */
static void ttcp_send_packets(struct ttcp_socket_t *sock)
{
  struct ttcp_msgqueue_t *cm;

  MUTEX_LOCK(&(sock->mutex));
  while (sock->sendq.next != &(sock->sendq)) {
    if (!sock->connected) 
      break;
    cm = sock->sendq.prev;
    if (sock->window < cm->header->datalen)
      break;
    if (ttcp_send_msg(sock, cm) == TTCP_SUCC) {
      DEBUG1("ttcp_send_packets: packet sent (seq: %d)\n", cm->header->seqnum);
      MSGQUEUE_REMOVE(cm);
      sock->sendc--;
      cm->age = 0;
      cm->retrans = 0;
      MSGQUEUE_INSERT(sock->ackq, cm);
      sock->ackc++;
      sock->unack_size += cm->header->datalen;
      sock->window -= cm->header->datalen;
    } else {
      DEBUG("ttcp_send_packets: ttcp_send_msg failed\n");
    }
  }
  if (sock->sendc < QUEUE_SIZE)
    if (pthread_cond_signal(&(sock->sendq_not_full)) != 0);
  MUTEX_UNLOCK(&(sock->mutex));
}


/*
 * wstawianie danych do bufora
 *
 * bufor jest cykliczny, wiec trzeba obslugiwac zawijanie
 */
static int ttcp_insert_to_buf(struct ttcp_socket_t *sock, const char *data, size_t count)
{
  int te;
  char *st;

  if (count > BUF_SIZE - sock->buf_data_len)
    return TTCP_ERR;
  
  te = BUF_SIZE - (sock->buf_data_start - sock->buf);
  st = sock->buf_data_start + sock->buf_data_len;
  if (st > sock->buf + BUF_SIZE)
    st = sock->buf + (sock->buf_data_len - te);
  te = (count > te) ? te : count;

  memcpy(st, data, te);

  if (count > te)
    memcpy(sock->buf, data + te, count - te);

  sock->buf_data_len += count;

  return TTCP_SUCC;
}


/*
 * odbieranie pakietow
 *
 * jesli zostal odebrany jakis pakiet, to:
 * - jesli oznacza koniec transmisji, to jezeli nie brakuje zadnych pakietow
 *   w strumieniu odsylane jest potwierdzenie konca, wpp. nic nie robimy.
 * - jesli jest znakiem zycia, to aktualizowana jest wartosc okna
 * - jesli jest to potwierdzenie, to z ackq usuwane sa wszystkie potwierdzone
 *   pakiety i ustawiana jest nowa wartosc okna
 * - jesli sa to dane, to w zaleznosci od seqnum i recvseq:
 *   - jesli seqnum == recvseq, to dane sa wstawiane do buf a nastepnie przegladana
 *     jest kolejka recvq i dane ze wszystkich kolejno nastepujacych po odebranym
 *     pakietow sa wstawiane do buf - na koncu wysylane jest potwierdzenie ostatniego
 *     pakietu wyrzuconego do bufora
 *   - wpp pakiet wstawiany jest do recvq i nie jest wysylane potwierdzenie.
 */
static void ttcp_recv_packets(struct ttcp_socket_t *sock)
{
  struct ttcp_msgqueue_t *cm, *cmnext;
  struct ttcp_msg_t *msg, *nmsg;
  char *data;
  char buf[MSG_SIZE];
  int n;

  if ((n = recv(sock->fd, buf, MSG_SIZE, 0)) >= (int)HEADER_SIZE) {
    DEBUG1("ttcp_recv_packets: packet received (size %d): ", n);
    sock->last_alive = time(0);
    msg = (struct ttcp_msg_t *)buf;
    switch (msg->type) {

    case TTCP_MSG_FIN:     /* koniec transmisji */
      if (sock->recvc == 0) {
	send(sock->fd, msg, HEADER_SIZE, 0);
	SETCMD(sock, TRANSCEIVER_CMD_FIN_NOASK);
	return;
      }

    case TTCP_MSG_ALIVE:   /* znak zycia    */
      DEBUG1("MSG_ALIVE (win: %d)\n", msg->window);
      sock->window = msg->window - sock->unack_size;
      break;

    case TTCP_MSG_CONFIRM: /* potwierdzenie */
      DEBUG1("MSG_CONFIRM (ack: %d)\n", msg->acknum);
      cm = sock->ackq.next;
      while (cm != &(sock->ackq)) {
	cmnext = cm->next;
	if (cm->header->seqnum + cm->header->datalen <= msg->acknum) {
	  MSGQUEUE_REMOVE(cm);
	  sock->ackc--;
	  sock->unack_size -= cm->header->datalen;
	  free(cm->header);
	  free(cm->data);
	  free(cm);
	}
	cm = cmnext;
      }
      sock->window = msg->window - sock->unack_size;
      break;

    case TTCP_MSG_DATA:    /* dane          */
      DEBUG1("MSG_DATA (seq: %d)\n", msg->seqnum);
      MUTEX_LOCK(&(sock->mutex));

      if (sock->recvseq == msg->seqnum) {
	DEBUG("ttcp_recv_packets: packet is next in sequence\n");
	if (ttcp_insert_to_buf(sock, buf + HEADER_SIZE, msg->datalen) == TTCP_SUCC) {
	  sock->recvseq += msg->datalen;

	  /* wstawianie do buf pakietow oczekujacych w recvq */
	  cm = sock->recvq.next;
	  while (cm != &(sock->recvq) && cm->header->seqnum == sock->recvseq) {
	    if (ttcp_insert_to_buf(sock, cm->data, cm->header->datalen) == TTCP_ERR)
	      break;
	    cmnext = cm->next;
	    sock->recvseq += cm->header->datalen;
	    MSGQUEUE_REMOVE(cm);
	    free(cm->header);
	    free(cm->data);
	    free(cm);
	    sock->recvc--;
	    cm = cmnext;
	  }

	  if ((nmsg = malloc(HEADER_SIZE)) != 0) {
	    nmsg->type = TTCP_MSG_CONFIRM;
	    nmsg->acknum = sock->recvseq;
	    nmsg->window = BUF_SIZE - sock->buf_data_len;
	    send(sock->fd, nmsg, HEADER_SIZE, 0);
	  }
	}
	if (pthread_cond_signal(&(sock->buf_is_data)) != 0);
      } else if (sock->recvseq < msg->seqnum) {
	DEBUG("ttcp_recv_packets: packet has to be queued\n");
	if (sock->recvc >= QUEUE_SIZE) {
	  MUTEX_UNLOCK(&(sock->mutex));
	  return;
	}
	if ((nmsg = malloc(HEADER_SIZE)) == 0) {
	  MUTEX_UNLOCK(&(sock->mutex));
	  return;
	}
	if ((data = malloc(msg->datalen)) == 0) {
	  free(nmsg);
	  MUTEX_UNLOCK(&(sock->mutex));
	  return;
	}
	if ((cm = malloc(sizeof(struct ttcp_msgqueue_t))) == 0) {
	  free(nmsg);
	  free(data);
	  MUTEX_UNLOCK(&(sock->mutex));
	  return;
	}
	
	memcpy(nmsg, msg, HEADER_SIZE);
	memcpy(data, buf + HEADER_SIZE, msg->datalen);
	cm->header = nmsg;
	cm->data = data;
	
	/* wstawianie do recvq zgodnie z seqnum */
	cmnext = sock->recvq.next;
	while (cmnext != &(sock->recvq) && cmnext->header->seqnum < msg->seqnum)
	  cmnext = cmnext->next;
	cm->prev = cmnext->prev;
	cm->next = cmnext;
	cmnext->prev->next = cm;
	cmnext->prev = cm;

	sock->recvc++;
      } else { /* przyszedl powtornie juz potwierdzony pakiet - potwierdzamy go jeszcze raz */
	if ((nmsg = malloc(HEADER_SIZE)) != 0) {
	  nmsg->type = TTCP_MSG_CONFIRM;
	  nmsg->acknum = sock->recvseq;
	  nmsg->window = BUF_SIZE - sock->buf_data_len;
	  send(sock->fd, nmsg, HEADER_SIZE, 0);
	}
      }

      MUTEX_UNLOCK(&(sock->mutex));
      break;

    default:
      DEBUG1("unexpected message type 0x%02x\n", msg->type);
      break;
    }
  }
}


/*
 * czyszczenie buforow
 *
 * usuwanie danych z sendq, recvq i ackq oraz zerowanie buf
 */
static void ttcp_clean_buffers(struct ttcp_socket_t *sock)
{
  struct ttcp_msgqueue_t *cm, *cmnext;

  DEBUG("ttcp_clean_buffers: cleaning up sendq\n");
  cm = sock->sendq.next;
  while (cm != &(sock->sendq)) {
    cmnext = cm->next;
    MSGQUEUE_REMOVE(cm);
    free(cm->header);
    free(cm->data);
    free(cm);
    cm = cmnext;
  }

  DEBUG("ttcp_clean_buffers: cleaning up ackq\n");
  cm = sock->ackq.next;
  while (cm != &(sock->ackq)) {
    cmnext = cm->next;
    MSGQUEUE_REMOVE(cm);
    free(cm->header);
    free(cm->data);
    free(cm);
    cm = cmnext;
  }

  DEBUG("ttcp_clean_buffers: cleaning up recvq\n");
  cm = sock->recvq.next;
  while (cm != &(sock->recvq)) {
    cmnext = cm->next;
    MSGQUEUE_REMOVE(cm);
    free(cm->header);
    free(cm->data);
    free(cm);
    cm = cmnext;
  }

  sock->sendc = sock->ackc = sock->recvc = sock->unack_size = 0;
  sock->recvseq = sock->sendseq = 0;
  sock->buf_data_len = 0;
  sock->buf_data_start = sock->buf;
}

/*
 * rozlaczanie awaryjne
 */
static void ttcp_disconnect(struct ttcp_socket_t *sock)
{
  MUTEX_LOCK(&(sock->mutex));
  if (!sock->connected) {
    MUTEX_UNLOCK(&(sock->mutex));
    return;
  }
  DEBUG("ttcp_disconnect: disconnecting...\n");
  sock->connected = 0;
  ttcp_clean_buffers(sock);
  pthread_cond_signal(&(sock->buf_is_data));
  pthread_cond_signal(&(sock->sendq_not_full));
  MUTEX_UNLOCK(&(sock->mutex));
}


/*
 * zarzadzanie kolejka ackq
 *
 * co sekunde postarza pakiety w kolejce ackq i rozsyla odpowiednio stare
 * oraz wysyla znak zycia
 * tutaj jest decydowane, czy uznac, ze ze drugi koniec nie zyje
 */
static void ttcp_manage_ackq(struct ttcp_socket_t *sock)
{
  static time_t lastupd = 0;
  struct ttcp_msgqueue_t *cm;
  struct ttcp_msg_t msg;

  if (lastupd == 0)
    lastupd = time(0);

  if (time(0) > lastupd) {

    if (time(0) - sock->last_alive > MAX_LATENCY) {
      ttcp_disconnect(sock);
      return;
    }
    /* wysylanie znaku zycia */
    msg.type = TTCP_MSG_ALIVE;
    MUTEX_LOCK(&(sock->mutex));
    msg.window = BUF_SIZE - sock->buf_data_len;
    MUTEX_UNLOCK(&(sock->mutex));
    send(sock->fd, &msg, HEADER_SIZE, 0);

    cm = sock->ackq.prev;
    while (cm != &(sock->ackq)) {
      cm->age++;
      if (cm->age > REPLY_TIMEOUT) {
	if (ttcp_send_msg(sock, cm) == TTCP_SUCC) {
	  DEBUG1("ttcp_manage_ackq: packet retransmitted (seq: %d)\n", cm->header->seqnum);
	  cm->age = 0;
	  cm->retrans++;
	  if (cm->retrans > RETRANSMIT_ATTEMPTS) {
	    ttcp_disconnect(sock);
	    return;
	  }
	}
      }
      cm = cm->prev;
    }
    lastupd = time(0);
  }
}


/* 
 * laczenie
 */
static int ttcp_manage_conn(struct ttcp_socket_t *sock)
{
  int win, i;
  char buf[MSG_SIZE];
  struct ttcp_msg_t *msg, *rmsg;
  unsigned char msgtype = TTCP_MSG_NONE;
  time_t t;

  MUTEX_LOCK(&(sock->mutex));
  if (sock->cmd != TRANSCEIVER_CMD_ACCEPT && sock->cmd != TRANSCEIVER_CMD_CONNECT)
    if (pthread_cond_wait(&(sock->do_connect), &(sock->mutex)) != 0) {
      DEBUG("ttcp_manage_conn: error: wait sock->do_connect\n");
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
  
  /* akceptowanie polaczenia */
  if (sock->cmd == TRANSCEIVER_CMD_ACCEPT) {
    while (1) {
      DEBUG("ttcp_manage_conn: waiting for MSG_CONNECT\n");
      do {
	msgtype = TTCP_MSG_NONE;
	if (recvfrom(sock->fd, buf, MSG_SIZE, 0, sock->rmtaddr, &(sock->addrlen)) == HEADER_SIZE) {
	  DEBUG("ttcp_manage_conn: received message\n");
	  msg = (struct ttcp_msg_t *)buf;
	  msgtype = msg->type;
	  win = msg->window;
	}
      } while (msgtype != TTCP_MSG_CONNECT);
      if ((msg = malloc(HEADER_SIZE)) == 0) {
	DEBUG("ttcp_manage_conn: error: malloc");
	MUTEX_UNLOCK(&(sock->mutex));
	return TTCP_ERR;
      }
      if (connect(sock->fd, sock->rmtaddr, sock->addrlen) != 0) {
	DEBUG("ttcp_manage_conn: connect failed\n");
	msg->type = TTCP_MSG_REFUSE;
	sendto(sock->fd, msg, HEADER_SIZE, 0, sock->rmtaddr, sock->addrlen);
	free(msg);
      } else {
	msg->type = TTCP_MSG_ACCEPT;
	msg->window = BUF_SIZE;
	send(sock->fd, msg, HEADER_SIZE, 0);
	free(msg);
	sock->window = win;
	sock->sendseq = sock->recvseq = 0;
	DEBUG1("ttcp_manage_conn: connected (window: %d)\n", win);
	break;
      }
    }

    /* wywolywanie serwera */
  } else if (sock->cmd == TRANSCEIVER_CMD_CONNECT) {
    if (connect(sock->fd, sock->rmtaddr, sock->addrlen) != 0) {
      DEBUG("ttcp_manage_conn: error: connect\n");
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
    if ((msg = malloc(HEADER_SIZE)) == 0) {
      DEBUG("ttcp_manage_conn: error: malloc\n");      
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
    msg->type = TTCP_MSG_CONNECT;
    msg->window = BUF_SIZE;
    DEBUG("ttcp_manage_conn: connecting...\n");
    send(sock->fd, msg, HEADER_SIZE, 0);
    t = time(0);
    i = 0;
    while (i <= CONNECT_ATTEMPTS) {
      if (recv(sock->fd, buf, MSG_SIZE, 0) == HEADER_SIZE) {
	DEBUG("ttcp_manage_conn: reply received\n");
	rmsg = (struct ttcp_msg_t *)buf;
	if (rmsg->type == TTCP_MSG_ACCEPT) {
	  sock->window = rmsg->window;
	  sock->sendseq = sock->recvseq = 0;
	  sock->last_alive = time(0);
	  DEBUG1("ttcp_manage_conn: connected (window: %d)\n", sock->window);
	  MUTEX_UNLOCK(&(sock->mutex));
	  return TTCP_SUCC;
	}
      }
      if (time(0) - t >= REPLY_TIMEOUT) {
	DEBUG("ttcp_manage_conn: reconnecting...\n");
	send(sock->fd, msg, HEADER_SIZE, 0);	
	t = time(0);
	i++;
      }
    }
    DEBUG("ttcp_manage_conn: connect timed out\n");
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  } 

  sock->last_alive = time(0);
  MUTEX_UNLOCK(&(sock->mutex));
  return TTCP_SUCC;
}

 
/* 
 * watek nadajnik/odbiornik
 *
 * po polaczeniu cyklicznie:
 * 1. wysyla pakiety z sendq
 * 2. probuje odebrac pakiet
 * 3. postarza pakiety w ackq i rozsyla odpowiednio stare
 */
static void *ttcp_transceiver(void *s)
{
  struct ttcp_socket_t *sock;
  struct ttcp_msg_t msg;
  time_t ts;
  char buf[MSG_SIZE];
  char cmd = TRANSCEIVER_CMD_NONE, con = 1;

  sock = (struct ttcp_socket_t *)s;

  while (cmd != TRANSCEIVER_CMD_STOP) {
    MUTEX_LOCK(&(sock->mutex));
    cmd = sock->cmd;
    con = sock->connected;
    MUTEX_UNLOCK(&(sock->mutex));

    if (cmd == TRANSCEIVER_CMD_FIN) {
      DEBUG("ttcp_transceiver: finalizing transmission\n");
      msg.type = TTCP_MSG_FIN;
      send(sock->fd, &msg, HEADER_SIZE, 0);
      ts = time(0);
      while (time(0) - ts < MAX_LATENCY) {
	if (recv(sock->fd, buf, MSG_SIZE, 0) > 0) {
	  if (((struct ttcp_msg_t *)buf)->type == TTCP_MSG_FIN) {
	    MUTEX_LOCK(&(sock->mutex));
	    sock->cmd = TRANSCEIVER_CMD_NONE;
	    sock->connected = 0;
	    con = 0;
	    pthread_cond_signal(&(sock->is_connected));
	    MUTEX_UNLOCK(&(sock->mutex));
	    break;
	  }
	}
      }
    } else if (cmd == TRANSCEIVER_CMD_FIN_NOASK) {
      DEBUG("ttcp_transceiver: finalizing transmission\n");
      MUTEX_LOCK(&(sock->mutex));
      sock->cmd = TRANSCEIVER_CMD_NONE;
      sock->connected = 0;
      con = 0;
      pthread_cond_signal(&(sock->is_connected));
      MUTEX_UNLOCK(&(sock->mutex));
    }
    
    if (con) {
      ttcp_send_packets(sock);
      ttcp_recv_packets(sock);
      ttcp_manage_ackq(sock);
    } else if (cmd == TRANSCEIVER_CMD_ACCEPT || cmd == TRANSCEIVER_CMD_CONNECT) {
      if (ttcp_manage_conn(sock) == TTCP_ERR) {
	MUTEX_LOCK(&(sock->mutex));
	sock->connected = 0;
	pthread_cond_signal(&(sock->is_connected));
	MUTEX_UNLOCK(&(sock->mutex));
	return 0;
      } else {
	MUTEX_LOCK(&(sock->mutex));
	sock->connected = 1;
	pthread_cond_signal(&(sock->is_connected));
	MUTEX_UNLOCK(&(sock->mutex));
      }
    }
  }

  return 0;
}


/*
 * tworzenie gniazda ttcp
 */
struct ttcp_socket_t *ttcp_socket()
{
  struct ttcp_socket_t *sock;
  pthread_attr_t attr;

  if ((sock = malloc(sizeof(struct ttcp_socket_t))) == 0)
    return 0;
  if ((sock->fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
    free(sock);
    return 0;
  }
  if (fcntl(sock->fd, F_SETFL, O_NONBLOCK) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_attr_init(&attr) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_mutex_init(&(sock->mutex), 0) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_cond_init(&(sock->buf_is_data), 0) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_cond_init(&(sock->sendq_not_full), 0) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_cond_init(&(sock->is_connected), 0) != 0) {
    free(sock);
    return 0;
  }
  if (pthread_cond_init(&(sock->do_connect), 0) != 0) {
    free(sock);
    return 0;
  }

  sock->window = 0;
  sock->connected = 0;
  sock->sendseq = sock->recvseq = 0;
  sock->sendq.next = sock->sendq.prev = &(sock->sendq);
  sock->recvq.next = sock->recvq.prev = &(sock->recvq);
  sock->ackq.next = sock->ackq.prev = &(sock->ackq);
  sock->sendc = sock->recvc = sock->ackc = 0;
  sock->unack_size = 0;
  sock->cmd = TRANSCEIVER_CMD_NONE;
  sock->buf_data_start = sock->buf;
  sock->buf_data_len = 0;

  if (pthread_create(&(sock->transceiver), &attr, ttcp_transceiver, (void *)sock) != 0) {
    free(sock);
    return 0;
 }
    
  return sock;  
}


/*
 * zwalnianie gniazda ttcp
 */
int ttcp_close(struct ttcp_socket_t *sock)
{
  MUTEX_LOCK(&(sock->mutex));
  if (!sock->connected) {
    DEBUG("ttcp_close: error: not connected\n");
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }
  MUTEX_UNLOCK(&(sock->mutex));

  MUTEX_LOCK(&(sock->mutex));
  sock->cmd = TRANSCEIVER_CMD_FIN;
  pthread_cond_wait(&(sock->is_connected), &(sock->mutex));
  sock->cmd = TRANSCEIVER_CMD_STOP;
  MUTEX_UNLOCK(&(sock->mutex));

  DEBUG("ttcp_close: closing fd\n");
  if (close(sock->fd) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: joining transceiver thread\n");
  if (pthread_join(sock->transceiver, 0) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: destroying mutex\n");
  if (pthread_mutex_destroy(&(sock->mutex)) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: destroying cond buf_is_data\n");
  if (pthread_cond_destroy(&(sock->buf_is_data)) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: destroying cond sendq_not_full\n");
  if (pthread_cond_destroy(&(sock->sendq_not_full)) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: destroying cond is_connected\n");
  if (pthread_cond_destroy(&(sock->is_connected)) != 0)
    return TTCP_ERR;
  DEBUG("ttcp_close: destroying cond do_connect\n");
  if (pthread_cond_destroy(&(sock->do_connect)) != 0)
    return TTCP_ERR;

  DEBUG("ttcp_close: cleaning buffers\n");
  ttcp_clean_buffers(sock);

  DEBUG("ttcp_close: freeing socket memory\n");
  free(sock);

  return TTCP_SUCC;
}


/*
 * ustawianie serwera w stan oczekiwania
 */
int ttcp_bind(struct ttcp_socket_t *sock, struct sockaddr *myaddr, socklen_t addrlen)
{
  return bind(sock->fd, myaddr, addrlen);
}


/*
 * akceptowanie polaczenia
 */
int ttcp_accept(struct ttcp_socket_t *sock, struct sockaddr *clntaddr, socklen_t *addrlen)
{
  MUTEX_LOCK(&(sock->mutex));

  if (sock->connected) {
    //DEBUG("ttcp_accept: error: already connected\n");
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }

  ttcp_clean_buffers(sock);
  sock->rmtaddr = clntaddr;
  sock->cmd = TRANSCEIVER_CMD_ACCEPT;
  if (pthread_cond_signal(&(sock->do_connect)) != 0) {
    DEBUG("ttcp_accept: error: signal sock->do_connect\n");
    MUTEX_UNLOCK(&(sock->mutex));  
    return TTCP_ERR;
  }
  if (pthread_cond_wait(&(sock->is_connected), &(sock->mutex)) != 0) {
    DEBUG("ttcp_accept: error: wait sock->is_connected\n");
    MUTEX_UNLOCK(&(sock->mutex));  
    return TTCP_ERR;
  }

  *addrlen = sock->addrlen;
  MUTEX_UNLOCK(&(sock->mutex));  

  if (sock->connected)
    return TTCP_SUCC;
  else
    return TTCP_ERR;
}


/*
 * nawiazywanie polaczenia
 */
int ttcp_connect(struct ttcp_socket_t *sock, struct sockaddr *servaddr, socklen_t addrlen)
{
  MUTEX_LOCK(&(sock->mutex));

  if (sock->connected) {
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }

  sock->rmtaddr = servaddr;
  sock->addrlen = addrlen;
  sock->cmd = TRANSCEIVER_CMD_CONNECT;
  if (pthread_cond_signal(&(sock->do_connect)) != 0) {
    MUTEX_UNLOCK(&(sock->mutex));  
    return TTCP_ERR;
  }
  if (pthread_cond_wait(&(sock->is_connected), &(sock->mutex)) != 0) {
    MUTEX_UNLOCK(&(sock->mutex));  
    return TTCP_ERR;
  }
  MUTEX_UNLOCK(&(sock->mutex));  

  if (sock->connected)
    return TTCP_SUCC;
  else
    return TTCP_ERR;
}


/*
 * czytanie z gniazda ttcp
 * 
 * dane sa odczytywane z sock->buf (o ile znajduja sie tam jakies dane;
 * wpp oczekiwanie na is_data)
 */
int ttcp_read(struct ttcp_socket_t *sock, void *buf, size_t count)
{
  size_t l, te;  

  MUTEX_LOCK(&(sock->mutex));
  if (!sock->connected) {
    DEBUG("ttcp_read: error: not connected\n");
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }

  if (sock->buf_data_len == 0)
    if (pthread_cond_wait(&(sock->buf_is_data), &(sock->mutex)) != 0) {
      DEBUG("ttcp_read: error: wait buf_is_data\n");
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }

  if (!sock->connected) {
    DEBUG("ttcp_read: error: not connected\n");
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }
  
  l = (sock->buf_data_len < count) ? sock->buf_data_len : count;
  te = BUF_SIZE - (sock->buf_data_start - sock->buf);
  te = (l < te) ? l : te;

  memcpy(buf, sock->buf_data_start, te);
  sock->buf_data_start += te;
  if (l > te) {
    memcpy(buf + te, sock->buf, l - te);
    sock->buf_data_start = sock->buf + (l - te);
  }
  if (sock->buf_data_start > sock->buf + BUF_SIZE)
    sock->buf_data_start = sock->buf;
  sock->buf_data_len -= l;

  MUTEX_UNLOCK(&(sock->mutex));

  return l;
}


/*
 * pisanie do gniazda ttcp
 *
 * dane z buf sa dzielone na pakiety i wstawiane do sendq, o ile jest
 * w niej miejsce; wpp oczekiwanie na sendq_not_full
 */
int ttcp_write(struct ttcp_socket_t *sock, const void *buf, size_t count)
{
  int st = 0, l;
  struct ttcp_msgqueue_t *cm;
  struct ttcp_msg_t *msg;
  char *data;

  MUTEX_LOCK(&(sock->mutex));

  if (!sock->connected) {
    MUTEX_UNLOCK(&(sock->mutex));
    return TTCP_ERR;
  }

  while (st < count) {
    if (sock->sendc >= QUEUE_SIZE) {
      DEBUG("ttcp_write: waiting for place in sendq\n");
      if (pthread_cond_wait(&(sock->sendq_not_full), &(sock->mutex)) != 0) {
	MUTEX_UNLOCK(&(sock->mutex));	
	return TTCP_ERR;
      }
    }
    if (!sock->connected) {
      DEBUG("ttcp_write: error: not connected\n");
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }

    l = (count - st > DATA_SIZE) ? DATA_SIZE : count - st;
    if ((msg = malloc(HEADER_SIZE)) == 0) {
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
    if ((data = malloc(l)) == 0) {
      free(msg);
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
    if ((cm = malloc(sizeof(struct ttcp_msgqueue_t))) == 0) {
      free(msg);
      free(data);
      MUTEX_UNLOCK(&(sock->mutex));
      return TTCP_ERR;
    }
    msg->type = TTCP_MSG_DATA;
    msg->datalen = l;
    msg->seqnum = sock->sendseq;
    memcpy(data, buf + st, l);
    cm->header = msg;
    cm->data = data;

    MSGQUEUE_INSERT(sock->sendq, cm);
    sock->sendc++;
    sock->sendseq += l;
    st += l;
  }

  MUTEX_UNLOCK(&(sock->mutex));

  return count;
}
