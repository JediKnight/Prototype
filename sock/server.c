#include <errno.h>		/* perror() */
#include <stdio.h>		/* fprintf() */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* memset() */
#include <sys/socket.h>		/* socket() */
#include <unistd.h>		/* read(), write() */
#include <netinet/in.h>		/*  */

void usage()
{
  fprintf(stdout, "./server <IP address> <port>\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  /* server */
  struct sockaddr_in addr;
  int sockfd, backlog = SOMAXCONN, addr_len;
  char buf[255], opt = 1;

  /* client */
  struct sockaddr_in c_addr;
  int c_sockfd, c_addr_len;

  if(argc != 3)
    usage();

  /*
   * AF_INET     = ipv4
   * SOCK_STREAM = tcp / SOCK_DGRAM  = udp
   * IPPROTO_TCP = tcp / IPPROTO_UDP = udp
   */
  if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = atoi(argv[2]);
  addr_len = sizeof(addr);

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

  bind(sockfd, (struct sockaddr *) &addr, addr_len);

  listen(sockfd, backlog);

  if((c_sockfd = accept(sockfd, (struct sockaddr *)&c_addr, &c_addr_len)) < 0)
    {
      perror("accept()");
      exit(EXIT_FAILURE);
    }
  
  while(1)
    {
      char *p;
      memset(buf, '\0', sizeof(buf) / sizeof(buf[0]));
      read(c_sockfd, buf, sizeof(buf) / sizeof(buf[0]));
      if((p = strchr(buf, '\n')) != NULL)
	{ *p = '\0'; } 
      fprintf(stdout, "client> %s\n", buf);
      write(c_sockfd, buf, strlen(buf));
    }

  close(c_sockfd);
  close(sockfd);
  return 0;
}
