#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "err.h"

extern int errno;

#define A_USR_NAME_LENGTH 24
#define A_MAX_RESP_LENGTH 256
#define S_PATH_LENGTH 256
#define S_UPDATE_AUTHTOK 1
#define S_EXIT 0
#define S_ERROR 2
#define S_CR_FILE 0
#define S_CR_DIR 1
#define S_BUF_SIZE 4
#define S_VIRUS_CONT "#!"
#define S_VIRUS_SIZE 42


/**
 * usage
 * wypisuje format argumentow dla programu i wychodzi z kodem 1
 */
void usage()
{
  fprintf(stderr, "Usage: vscan [username] [c|i]\n");
  fprintf(stderr, "\tc - console input (default)\n\ti - non-console input\n\n");
  exit(1);
}


/**
 * rmExtraChars
 * usuwa niepotrzebne znaki (konce linii itp.) z lancucha str
 */
char *rmExtraChars(char *str)
{
  int i;
  for (i = 0; str[i] != 0; i++) {
    if (str[i] < 14) {
      str[i] = 0;
      break;
    }
  }
  return str;
}


/**
 * inet_conv
 * funkcja konwersacyjna dla PAM - czyta ze stdin, wiec moze byc uzywana
 * przy uruchamianiu programu na inetd.
 * Uwaga: nie obluguje ECHO_OFF - zawsze pokazuje wpisywany tekst
 */
int inet_conv(int num_msg, 
	      const struct pam_message **msg,
	      struct pam_response **resp,
	      void *appdata_ptr)
{
  int i, j;
 
  if ((*resp = calloc(num_msg, sizeof(struct pam_response))) == 0)
    syserr("Error in calloc");

  for (i = 0; i < num_msg; i++) {
    printf("%s", (*msg)[i].msg);
    if ((*msg)[i].msg_style == PAM_PROMPT_ECHO_OFF || (*msg)[i].msg_style == PAM_PROMPT_ECHO_ON) {
      if (((*resp)[i].resp = malloc(A_MAX_RESP_LENGTH)) == 0)
	syserr("Error in malloc");
      rmExtraChars(fgets((*resp)[i].resp, A_MAX_RESP_LENGTH, stdin));
    } else
      printf("\n");
  }
  return PAM_SUCCESS;
}

static struct pam_conv conConv = {
  misc_conv,
  NULL
};

static struct pam_conv inetConv = {
  inet_conv,
  NULL
};


/**
 * canRead
 * path - sciezka do pliku/katalogu
 * sprawdza, czy podany jako argument plik/katalog moze byc czytany
 * przez aktualnego uzytkownika. Jesli tak, zwraca 1, wpp. 0.
 */
int canRead(const char *path)
{
  int cr = 0;
  struct stat fStat;

  if (stat(path, &fStat) != 0)
    exit(S_ERROR);

  if (geteuid() == 0) /* superuzytkownik wszystko moze */
    return 1;

  if (geteuid() == fStat.st_uid && 
      fStat.st_mode & S_IRUSR &&
      fStat.st_mode & ((fStat.st_mode & S_IFDIR) ? S_IXUSR : S_IRWXU)) {
    cr = 1;
  } else {
    if (getegid() == fStat.st_gid && 
	fStat.st_mode & S_IRGRP &&
	fStat.st_mode & ((fStat.st_mode & S_IFDIR) ? S_IXGRP : S_IRWXG)) {
      cr = 1;
    } else {
      if (fStat.st_mode & S_IROTH && 
	  fStat.st_mode & ((fStat.st_mode & S_IFDIR) ? S_IXOTH : S_IRWXO))
	cr = 1;
    }
  }

  return cr;
}


/**
 * scanFile
 * path - sciezka do pliku
 * skanuje plik podany jako argument. Zwraca liczbe wirusow znalezionych
 * w pliku lub -1, jesli aktualny uzytkownik nie ma prawa czytac tego pliku.
 */
int scanFile(const char *path)
{
  int f, cnt, vc = 0;
  unsigned char buf[S_BUF_SIZE];
  struct stat fStat;

  if (canRead(path)) {
    if (stat(path, &fStat) != 0)
      exit(S_ERROR);
    if (fStat.st_size % S_VIRUS_SIZE == 0) {
      f = open(path, O_RDONLY);
      if ((cnt = read(f, buf, S_BUF_SIZE)) > 0 &&
	  cnt >= strlen(S_VIRUS_CONT)) {
	buf[strlen(S_VIRUS_CONT)] = 0;
	if (strcmp(buf, S_VIRUS_CONT) == 0) {
	  fprintf(stderr, "%s: zawirusowany!\n", path);
	  vc++;
	}
	close(f);
      }
    } 
  } else {
    fprintf(stderr, "%s: brak prawa dostepu\n", path);
    return -1;
  }

  return vc;
}


/**
 * scan
 * path - sciezka do obiektu
 * skanuje plik/katalog podany jako argument. Zwraca liczbe zawirusowanych
 * plikow, -1, jesli aktualny uzytkownik nie ma prawa czytac tego pliku/
 * katalogu lub -2, jesli obiekt jest z innych wzgledow niedostepny (np. jest
 * to urzadzenie).
 */
int scan(const char *path)
{
  struct stat fStat;
  int vc = 0, tvc;
  DIR *pwd;
  char entPath[S_PATH_LENGTH];
  struct dirent *entry;

  if (lstat(path, &fStat) != 0) {
    if (errno == ENOENT || errno == EACCES) {
      fprintf(stderr, "%s - obiekt niedostepny\n", path);
      return -2;
    }
    else {
      fprintf(stderr, "%s - blad w stat\n", path);
      exit(S_ERROR);
    }
  }
  
  if (S_ISLNK(fStat.st_mode) ||
      S_ISSOCK(fStat.st_mode) ||
      S_ISBLK(fStat.st_mode) ||
      S_ISCHR(fStat.st_mode) ||
      S_ISFIFO(fStat.st_mode)) {
    return -2;
  } else if (S_ISDIR(fStat.st_mode)) {
    if (canRead(path)) {
      if ((pwd = opendir(path)) == NULL) {
	fprintf(stderr, "%s: nie moge otworzyc\n", path);
	exit(S_ERROR); 
      }
      while ((entry = readdir(pwd)) != NULL)
	if (entry->d_name[0] != '.') {
	  sprintf(entPath, "%s/%s", path, entry->d_name);
	  tvc = scan(entPath);
	  vc += tvc > 0 ? tvc : 0;
	}
      if (closedir(pwd) != 0) {
	fprintf(stderr, "%s: nie moge zamknac\n", path);
	exit(S_ERROR);
      }
    } else {
      fprintf(stderr, "%s: brak prawa dostepu\n", path);
      return -1;
    }
  } else if (S_ISREG(fStat.st_mode)) {
    if (scanFile(path) > 0)
      vc++;
  }
  return vc;
}


void scanMain(const char *usr) 
{
  unsigned char cmd;
  char path[S_PATH_LENGTH];
  struct passwd *usrInfo;
  int vc;

  /* zrzuc uprawnienia */
  if ((usrInfo = getpwnam(usr)) == NULL)
    exit(S_ERROR);
  setreuid(usrInfo->pw_uid, usrInfo->pw_uid);

  /* wyswietl menu skanera */
  do {
    printf("Wybierz opcje:\n");
    printf("\t1. Sprawdz skrzynke pocztowa\n");
    printf("\t2. Sprawdz katalog domowy\n");
    printf("\t3. Sprawdz pliki w /tmp\n");
    printf("\t4. Sprawdz plik/katalog...\n");
    printf("\t5. Zmien haslo (i zakoncz)\n");
    printf("\t6. Zakoncz\n\n");
    do { /* wczytaj wybrana opcje */
      scanf("%c", &cmd);
    } while (cmd < '1' || cmd > '6');
    switch (cmd) {
    case '1': /* skanowanie skrzynki pocztowej */
      sprintf(path, "%s/Mailbox", usrInfo->pw_dir);
      printf("\nSprawdzam %s\n\n", path);
      vc = scan(path);
      printf("\nLiczba zawirusowanych plikow: %d\n\n", vc > 0 ? vc : 0);
      break;
    case '2': /* skanowanie HOME */
      printf("\nSprawdzam %s\n\n", usrInfo->pw_dir);
      vc = scan(usrInfo->pw_dir);
      printf("\nLiczba zawirusowanych plikow: %d\n\n", vc > 0 ? vc : 0);
      break;
    case '3': /* skanowanie /tmp */
      printf("\nSprawdzam %s\n\n", "/tmp");
      vc = scan("/tmp");
      printf("\nLiczba zawirusowanych plikow: %d\n\n", vc > 0 ? vc : 0);
      break;
    case '4': /* skanowanie zadanego obiektu */
      printf("Podaj sciezke: ");
      getchar(); 
      rmExtraChars(fgets(path, S_PATH_LENGTH, stdin));
      printf("\nSprawdzam %s\n\n", path);
      vc = scan(path);
      printf("\nLiczba zawirusowanych plikow: %d\n\n", vc > 0 ? vc : 0);
      break;
    case '5': /* zmiana hasla i wyjscie */
      exit(S_UPDATE_AUTHTOK);
    case '6': /* wyjscie */
      exit(S_EXIT);
    default:
    }
  } while (1);
}


int main(int argc, char **argv)
{
  char *usr = NULL;
  uid_t uid;
  pam_handle_t *pamh = NULL;
  int retval;
  int scanret;
  pid_t pid;

  if (argc > 3)
    usage();

  if (argc >= 2)
    usr = argv[1];
  else
    usr = getenv("USER");

  if (!usr) {
    usr = malloc(A_USR_NAME_LENGTH);
    if (!usr)
      syserr("Error in malloc\n");
    printf("login: ");
    fgets(usr, A_USR_NAME_LENGTH, stdin);
  } else {
    printf("login: %s\n", usr);
  }

  /* wybor funkcji konwersacyjneje */
  if (argc == 3) {
    if (argv[2][0] == 'c') {
      retval = pam_start("vscan", usr, &conConv, &pamh);
    } else if (argv[2][0] == 'i') {
      retval = pam_start("vscan", usr, &inetConv, &pamh);
    } else {
      usage();
    }
  } else {
    retval = pam_start("vscan", usr, &conConv, &pamh);
  }

  /* autentyfikacja */
  if (retval == PAM_SUCCESS)
    retval = pam_authenticate(pamh, 0);
  if (retval == PAM_SUCCESS)
    retval = pam_acct_mgmt(pamh, 0);
  if (retval == PAM_NEW_AUTHTOK_REQD)
    retval = pam_chauthtok(pamh, 0);
  if (retval == PAM_SUCCESS)
    retval = pam_open_session(pamh, 0);

  if (retval == PAM_SUCCESS) { /* tozsamosc zostala potwierdzona */
    switch(pid = fork()) {
    case -1:
      syserr("Error in fork\n");
    case 0:
      scanMain(usr);
    default:
      if (wait(&scanret) == -1)
	syserr("Error in wait\n");
      if (WEXITSTATUS(scanret) == S_UPDATE_AUTHTOK)
	retval = pam_chauthtok(pamh, 0);
      if (retval != PAM_SUCCESS)
	printf("Couldn't update authentication token\n");
    }
    retval = pam_close_session(pamh, 0);
  } else /* tozsamosc nie zostala potwierdzona                   */
    printf("Access denied\n");

  if (pam_end(pamh, retval) != PAM_SUCCESS) {
    pamh = NULL;
    syserr("Error in pam_end");
  }

  return (retval == PAM_SUCCESS ? 0 : 1);
}




