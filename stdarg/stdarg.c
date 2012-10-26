#include <stdio.h>
#include <stdlib.h>

void variablearg(int argnm, ...)
{
  va_list args;
  int i, val;
  va_start(args, argnm);
  for(i = 0; i < argnm; i++)
    {
      val = va_arg(args, int);
      printf("%d ", val);
    }
  printf("\n");
  va_end(args);
}

int main()
{
  variablearg(3, 1, 2, 3);
  return 0;
}
