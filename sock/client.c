#include <errno.h>		/* perror() */
#include <arpa/inet.h>		/* inet_addr() */
#include <stdio.h>		/* fprintf() */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* memset() */
#include <sys/socket.h>		/* socket(), bind(), connect() */
#include <netinet/in.h>		/*  */
#include <unistd.h>		/*  */
#include "mystring.h"

#undef BUFSIZ
#define BUFSIZ 255
#undef UNAME
#define UNAME getenv("USER")
#undef UNAME_LEN
#define UNAME_LEN strlen(UNAME)

void usage()
{
  fprintf(stdout, "./client <IP address> <port>\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  int sockfd, len;
  struct sockaddr_in addr;

  if(argc != 3)
    usage();

  /**
   *
   *
   */
  if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(atoi(argv[2]));
  len = sizeof(addr);

  /**
   * int
   * connect(int socket, const struct sockaddr *address, socklen_t address_len);
   */
  if(connect(sockfd, (const struct sockaddr *)&addr, len) < 0)
    {
      perror("connect()");
      exit(EXIT_FAILURE);
    }

  for(;;)
    {
      char buf[BUFSIZ], mesg[BUFSIZ + UNAME_LEN];
      memset(buf, '\0', BUFSIZ);
      memset(mesg, '\0', BUFSIZ + UNAME_LEN);

      fgets(buf, BUFSIZ, stdin);
      goodbyeReturn(buf);
      sprintf(mesg, "%s> %s\n", UNAME, buf);
      write(sockfd, mesg, strlen(mesg));

      memset(mesg, '\0', BUFSIZ + UNAME_LEN);
      read(sockfd, mesg, BUFSIZ + UNAME_LEN);
      fprintf(stdout, "%s\n", mesg);
    }

  close(sockfd);
  return 0;
}
