/*
 lacze nienazwane (pipe) - rura jednokierunkowa
 jeden do niej pisze, drugi z niej czyta ciagi bajtow
 to jest tak naprawde plik bez nazwy (i-node)
 
 pipe( int fd[2]) => f[0] = czytanie, f[1] = pisanie.
 jak teraz wyforkujemy syna, to oba procesy beda mialy te
 dwa pliki otwarte.
 (wszystkie otwarte pliki sa dziedziczone)
 
 np chcemy: ojciec =>pipe=> syn
 
 zamykamy po forku deskryptor czytania, a u syna zamykamy 
 deskryptor pisania.

 czytanie i pisanie - jak do pliku (read/write)
 pojemnosc pipe'a: >= 4096 bajtow.
 uwaga: jak nie bedzie zadnego czytelnika, to wywola sie broken pipe 

*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void syn()
{
 int i;
 for(i=1;i<10;i++) printf("s%d", i);
}

int main()
{
 int i;
 pid_t pid;

 switch (pid = fork())
 {
  case -1:
    exit(1);
  case 0:
    for(i=1;i<10;i++) printf("s%d", i);
    return(0);
  default:
    wait(0);
    for(i=1;i<10;i++) printf("o%d", i);
    return(0);
 }
}