#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "error.h"

int main()
{
  pid_t pid;

  switch(pid = fork())
  {
  case -1: //blad
    report_error("Error in fork\n");
    exit(1);
  case 0: //syn
    return(0);
  default: //ociec
    return(0);
  }
  return 0;
}
