#include <errno.h>		/* perror() */
#include <arpa/inet.h>		/* inet_addr() */
#include <stdio.h>		/* fprintf() */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* memset() */
#include <sys/socket.h>		/* socket(), bind(), connect() */
#include <netinet/in.h>		/*  */
#include <unistd.h>		/*  */

void usage()
{
  fprintf(stdout, "./client <IP address> <port>\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  int sockfd, len;
  struct sockaddr_in addr;
  char buf[255];

  if(argc != 3)
    usage();

  if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = atoi(argv[2]);
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
      char *p;
      int blen;
      memset(buf, '\0', sizeof(buf) / sizeof(buf[0]));
      fgets(buf, sizeof(buf) / sizeof(buf[0]), stdin);
      if((p = strchr(buf, '\n')) != NULL)
	{ *p = '\0'; } 
      blen = strlen(buf);
      write(sockfd, buf, blen);
      memset(buf, '\0', sizeof(buf) / sizeof(buf[0]));
      read(sockfd, buf, blen);
      fprintf(stdout, "%s\n", buf);
    }

  close(sockfd);
  return 0;
}
