#include <string.h>

void report_error(char* errmsg)
{
  write(2, errmsg, strlen(errmsg));
}
