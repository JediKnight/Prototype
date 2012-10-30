#include <stdio.h>
#include <string.h>

#undef BUFSIZ
#define BUFSIZ 255

struct {
  char fname[BUFSIZ];
  char lname[BUFSIZ];
  int age;
  _list *prev;
  _list *next;
} _list;

#define clear(a)				\
  memset(a, '\0', BUFSIZ)

#define getChar(a)				\
  fgets(a, BUFSIZ, stdin)

int main()
{
  struct _list *list;
  int i;
  for(i = 0;i < 3; i++)
    {
      char buf[BUFSIZ];

      clear(buf);
      printf("名前を入力> ");
      getChar(buf);
      list->fname = buf;

      clear(buf);
      printf("苗字を入力> ");
      getChar(buf);
      list->lname = buf;

      clear(buf);
      printf("年齢を入力> ");
      getChar(buf);
      list->age = atoi(buf);
    }
  return 0;
}

  
