#include <errno.h>		/*  */
#include <stdio.h>		/*  */
#include <stdlib.h>		/*  */
#include <string.h>		/* memset() */
#include <sys/socket.h>		/*  */
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
  char s[255], r[255], opt = 1;

  if(argc != 3)
    usage();

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = atoi(argv[2]);
  len = sizeof(addr);

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

  if(connect(sockfd, (struct sockaddr *)&addr, len) == -1)
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
