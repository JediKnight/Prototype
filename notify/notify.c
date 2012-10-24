#include <stdio.h>		/* perror(), printf() */
#include <stdlib.h>		/* exit() */
#include <sys/wait.h>		/* wait() */
#include <netinet/in.h>		/* IPPROTO_TCP */
#include <sys/socket.h>		/* socket(), bind(), listen(), accept() */
#include <string.h>		/* bzero() */
#include <signal.h>		/* kill() */
#include <unistd.h>		/* close() */
#include <ncurses.h>		/* initscr(), printw(), enwin() */

#undef IPADDR
#define IPADDR "127.0.0.1"
#undef PORTNM
#define PORTNM 19860

int serverSocket()
{
  struct sockaddr_in address;
  int sockfd, opt, len;

  if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    { perror("socket()"); return -1; }

  bzero((char *)&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(IPADDR);
  address.sin_port = htons(PORTNM);
  len = sizeof(address);

  opt = 1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt)) == -1)
    { perror("setsockopt()"); return -1; }

  if(bind(sockfd, (const struct sockaddr *)&address, (socklen_t)len) == -1)
    { perror("bind()"); return -1; }

  return sockfd;
}

int closeSocket(int sockfd)
{
  if(close(sockfd) == -1)
    return -1;
  return 0;
}

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
  //  overlay(stdscr, wnd);
  box(wnd, ACS_VLINE, ACS_HLINE);
  //  wborder(wnd, '|', '|', '-', '', '+', '+', '+', '+');
  wbkgd(wnd, COLOR_PAIR(2));

  for(;;)
    {
      char buf[BUFSIZ], *p;
      memset(buf, '\0', BUFSIZ);
      recv(sockfd, buf, BUFSIZ, 0);
      if((p = strchr(buf, '\n') -1) != NULL) *p = '\0';
      if(strcmp(buf, "exit") == 0) break;
      wprintw(wnd, "%s\n", buf);
      wrefresh(wnd);
    }
  endwin();
}

void child()
{
  int sockfd, client_sockfd, len, rsize;
  struct sockaddr_in addr;

  if((sockfd = serverSocket()) == -1)
    { fprintf(stderr, "serverSocket"); exit(EXIT_FAILURE); }

  if(listen(sockfd, SOMAXCONN) == -1)
    { perror("listen"); closeSocket(sockfd); exit(EXIT_FAILURE); }

  switch(client_sockfd = accept(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len))
    {
    case -1:
      perror("accept");
      closeSocket(sockfd);
      closeSocket(client_sockfd);
      exit(EXIT_FAILURE);
      
    default:
      mesgWnd(client_sockfd);
      break;
    }

  closeSocket(sockfd);
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
