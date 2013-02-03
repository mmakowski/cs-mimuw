#ifndef COMMON_H
#define COMMON_H

#define TRACE(s) //fprintf(stderr, "%s\n", (s))

#define MALLOC(d, s) if (((d) = malloc(s)) == 0) syserr("malloc")
#define CALLOC(d, c, s) if (((d) = calloc((c), (s))) == 0) syserr("calloc")

#endif
