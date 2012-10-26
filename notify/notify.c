#include <stdio.h>		/* perror(), printf() */
#include <stdlib.h>		/* exit() */
#include <sys/wait.h>		/* wait() */
#include <netinet/in.h>		/* IPPROTO_TCP */
#include <sys/socket.h>		/* socket(), bind(), listen(), accept() */
#include <string.h>		/* bzero() */
#include <signal.h>		/* kill() */
#include <unistd.h>		/* close() */
#include <ncurses.h>		/* initscr(), printw(), enwin() */
#include "sc_socket.h"

int initCurses()
{
  initscr();
  noecho();
  nocbreak();
  start_color();
  use_default_colors();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  return 0;
}

#define LINE 2
void mesgWnd(int sockfd)
{
  WINDOW *wnd;
  int max_x, max_y, begin_x, begin_y, ncols, nlines;

  initCurses();

  getmaxyx(stdscr, max_y, max_x);
  begin_y = 0;
  begin_x = 0;
  ncols = 35;
  nlines = max_y;

  wnd = newwin(nlines, ncols, begin_y, begin_x);
  wbkgd(wnd, COLOR_PAIR(2));

  for(;;)
    {
      char buf[BUFSIZ], *p;
      recvdata(sockfd, buf, BUFSIZ);
      if(strcmp(buf, "exit") == 0) break;
      wprintw(wnd, "%s\n", buf);
      wrefresh(wnd);
    }
  endwin();
}

void child()
{
  int server_sockfd, client_sockfd;

  if((server_sockfd = serverSocket()) == -1)
    { fprintf(stderr, "serverSocket"); exit(EXIT_FAILURE); }

  if(listen(server_sockfd, SOMAXCONN) == -1)
    { perror("listen"); closeSocket(server_sockfd); exit(EXIT_FAILURE); }

  if((client_sockfd = clientSocket(server_sockfd)) == -1)
    { fprintf(stderr, "clientSocket"); exit(EXIT_FAILURE); }

  mesgWnd(client_sockfd);

  closeSocket(server_sockfd);
  closeSocket(client_sockfd);

  puts("process end");
  exit(EXIT_SUCCESS);
}

void parent()
{
  puts("process start");
}

int main()
{
  pid_t pid;
  switch(pid = fork())
    {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0:
      child();
      break;

    default:
      parent();
      break;
    }

  return 0;
}
