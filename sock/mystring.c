#include <string.h>
#include "mystring.h"

void goodbyeReturn(char *val)
{
  char *p;
  if((p = strchr(val, '\n')) != NULL)
    *p = '\0';
}
