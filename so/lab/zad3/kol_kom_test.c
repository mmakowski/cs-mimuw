/*
 * obsluga kolejek komunikatow
 * rozwiazanie zad. 3 z SO LAB
 * autor: Maciek Makowski (mm189188)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define STR_LEN 128

int ar, aw, ur, uw, all, user;

/* czytanie prywatnego komunikatu */
void read_user()
{
  int len;
  char msg[128 + 1];

  len = read(ur, msg, 128);
  msg[len] = 0;
  printf("%s\n", msg);
}

/* ustawianie adresata */
int set_dest(int dest)
{
  int rv;

  rv = ioctl(uw, dest, 0);
  return rv;
}

/* pisanie prywatnego komunikatu */
void write_user(char *msg, int size)
{
  int len;

  len = write(uw, msg, size);
}


/* czytanie publicznego komunikatu */
void read_all()
{
  int len;
  char msg[128 + 1];

  len = read(ar, msg, 128);
  msg[len] = 0;
  printf("%s\n", msg);
}

/* pisanie publicznego komunikatu */
void write_all(char *msg, int size)
{
  write(aw, msg, size);
}

/* ustawianie rozmiaru kolejki */
void set_size(int size)
{
  char msg[10];
  sprintf(msg, "size %d ", size);
  write(all, msg, strlen(msg));
}

/* zatrzymywanie kolejek */
void stop()
{
  write(all, "stop", 4);
}

/* uruchamianie kolejek */
void start()
{
  write(all, "start", 5);
}

/* czyszczenie kolejek */
void clean()
{
  write(all, "clean", 5);
}

/* dodawanie uzytkownika */
void add_user(int uid)
{
  char msg[10];
  sprintf(msg, "add %d ", uid);
  write(user, msg, strlen(msg));
}

/* usuwanie uzytkownika */
void del_user(int uid)
{
  char msg[10];
  sprintf(msg, "del %d ", uid);
  write(user, msg, strlen(msg));
}

char *getstr(char *msg)
{
  char *str;
  if ((str = malloc(STR_LEN + 1)) == 0) {
    fprintf(stderr, "brak pamieci\n");
    return 0;
  }
  printf("%s", msg);
  fgets(str, STR_LEN, stdin);
  return str;
}

int getint(char *msg)
{
  int i;
  printf("%s", msg);
  scanf("%d", &i);
  getchar();
  return i;
}

int main()
{
  unsigned char c;
  char *msg;

  ar = open("/dev/all_read", O_RDONLY);
  aw = open("/dev/all_write", O_WRONLY);  
  ur = open("/dev/user_read", O_RDONLY);
  uw = open("/dev/user_write", O_WRONLY);  

  all = open("/proc/komunikaty/all", O_WRONLY);
  user = open("/proc/komunikaty/user", O_WRONLY);

  do { 
    printf("\nZarzadzanie kolejkami komunikatow\n\n");
    printf("\t[1] odebranie prywatnego komunikatu\n");
    printf("\t[2] wyslanie prywatnego komunikatu\n");
    printf("\t[3] zmiana adresata prywatnego komunikatu\n");
    printf("\t[4] odebranie publicznego komunikatu\n");
    printf("\t[5] wyslanie publicznego komunikatu\n");
    printf("\t-----------------------------------------\n");
    if (all != -1) {
      printf("\t[r] uruchomienie systemu kolejek\n");
      printf("\t[s] zatrzymanie systemu kolejek\n");
      printf("\t[c] czyszczenie wszystkich kolejek\n");
      printf("\t[l] ustawienie rozmiaru kolejek\n");
      printf("\t[a] dodanie uzytkownika\n");
      printf("\t[d] usuniecie uzytkownika\n");
      printf("\t-----------------------------------------\n");
    }
    printf("\t[q] koniec\n\n> ");

    c = getchar(); getchar();
    switch (c) {
    case '1':
      read_user();
      break;
    case '2':
      msg = getstr("podaj komunikat: ");
      write_user(msg, strlen(msg));
      break;
    case '3':
      set_dest(getint("podaj uid adresata: "));
      break;
    case '4':
      read_all();
      break;
    case '5':
      msg = getstr("podaj komunikat: ");
      write_all(msg, strlen(msg));
      break;
    default:
      if (all != -1) {
	switch (c) {
	case 'r':
	  start();
	  break;
	case 's':
	  stop();
	  break;
	case 'c':
	  clean();
	  break;
	case 'l':
	  set_size(getint("podaj rozmiar kolejki: "));
	  break;
	case 'a':
	  add_user(getint("podaj uid uzytkownika: "));
	  break;
	case 'd':
	  del_user(getint("podaj uid uzytkownika: "));
	  break;
	default:
	}
      }
    }
  } while (c != 'q');
  close(ar);
  close(aw);
  close(ur);
  close(uw);

  return 0;
}













