#ifndef _ERR_
#define _ERR_

/* print system call error message and terminate */
extern void syserr(const char *fmt, ...);

/* print error message and terminate */
extern void fatal(const char *fmt, ...);

#endif
