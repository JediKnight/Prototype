#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#undef BUFSIZ
#define BUFSIZ 255

struct {
  char fname[BUFSIZ];
  char lname[BUFSIZ];
  int age;
  struct _list *prev;
  struct _list *next;
} _list;

#define clear(a)				\
  memset(a, '\0', BUFSIZ)

#define getChar(a)				\
  fgets(a, BUFSIZ, stdin);			\
  if(strcmp(a, "\n") == 0)			\
    a[sizeof(a) / sizeof(a[0])] = '\0'

int main()
{
  struct _list *list;
  int i;

  for(i = 0;i < 3; i++)
    {
      struct _list *p;
      char fn[BUFSIZ], ln[BUFSIZ], ag[BUFSIZ];

      p = (struct _list *)malloc(sizeof(struct _list *));

      printf("名前を入力> ");
      getChar(fn);

      printf("苗字を入力> ");
      getChar(ln);

      printf("年齢を入力> ");
      getChar(ag);

      strncpy(p->fname, fn, BUFSIZ);
      strncpy(p->lname, ln, BUFSIZ);
      p->age = atoi(ag);
      p->prev = NULL;
      p->next = NULL;
      list = p;
    }
  return 0;
}

  
