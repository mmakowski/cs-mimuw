#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
 int i;
 pid_t pid;
 int fk = 0;
 int min, mout, bin, bout, tmp[2];
 char buf[255];
 
 min = 0; 
 
 pipe(tmp);
 bin = min;
 bout = tmp[1];
 mout = 1;
 min = tmp[2];
 write(bout, "123456789012345678901234567890123456\n", 37);

 for (i = 1; i < 6 && !fk; i++)
 {
  switch(pid = fork())
  {
   case -1:
     exit(1);
   case 0: /* syn */
     read(min, buf, 37); 
     pipe(tmp);
     bin = min;
     bout = tmp[1];
     mout = 1;
     min = tmp[2];
     break;
   default: /* ociec */
     mout = bout; min = bin; //if i=5...
     write(bout, buf, 37);
     fk = 1;
  }
 }
 return 0;
}