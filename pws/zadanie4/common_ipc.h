#ifndef COMMON_IPC_H
#define COMMON_IPC_H

#define SEMGET(t, x, y, z)\
if (((t) = semget((x), (y), (z))) == -1)\
syserr("semget")
#define MSGGET(t, x, y)\
if (((t) = msgget((x), (y))) == -1)\
syserr("msgget")
#define SHMGET(t, x, y, z)\
if (((t) = shmget((x), (y), (z))) == -1)\
syserr("shmget")
#define SEMCTL(t, x, y, z)\
if (semctl((t), (x), (y), (z)) == -1)\
syserr("semctl")
#define SEMOP(s, x, y)\
if (semop((s), (x), (y)) == -1)\
syserr("semop")
#define SEM_P(s, n)\
sb.sem_num = (n);\
sb.sem_op = -1;\
sb.sem_flg = 0;\
SEMOP((s), &sb, 1)
#define SEM_V(s, n)\
sb.sem_num = (n);\
sb.sem_op = 1;\
sb.sem_flg = 0;\
SEMOP((s), &sb, 1)
#define MSGRCV(q, b, s, t)\
if (msgrcv((q), (b), (s), (t), 0) == -1)\
syserr("msgrcv")
#define SHMCTL(t, x, y)\
if (shmctl((t), (x), (y)) == -1)\
syserr("shmctl")
#define SHMAT(t, x)\
if ((t = (char *)shmat((x), 0, 0)) == (char *)-1)\
syserr("shmat")
#define SHMDT(a)\
if (shmdt(a) == -1)\
syserr("shmdt")


#define BAZA_SHM_KEY 2323L
#define BAZA_SEM_KEY 2000L

#define SEM_W_ZESTAWIE 100

#endif
