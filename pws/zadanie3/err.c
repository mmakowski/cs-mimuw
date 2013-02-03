#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include "err.h"

void syserr(const char *fmt, ...)  
{
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");

  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end (fmt_args);

  fprintf(stderr," (%d", errno);
  if (errno > 0 && errno < sys_nerr)
    fprintf(stderr,"; %s)\n", sys_errlist[errno]);
  else
    fprintf(stderr, ")\n");
  exit(1);
}

void fatal(const char *fmt, ...)
{
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");

  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end (fmt_args);

  fprintf(stderr,"\n");
  exit(1);
}
