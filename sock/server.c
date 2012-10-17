#include <arpa/inet.h>		/* inet_addr() */
#include <errno.h>		/* perror() */
#include <netinet/in.h>		/* htons() */
#include <pthread.h>            /* pthread_create(), pthread_detach() */
#include <stdio.h>		/* fprintf() */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* memset() */
#include <sys/fcntl.h>		/* open() */
#include <sys/sem.h>		/* semget() */
#include <sys/socket.h>		/* socket(), bind(), listen(), recv() */
#include <sys/stat.h>		/* mkfifo() */
#include <time.h>		/*  */
#include <unistd.h>		/* read(), write() */
#include "mystring.h"

#undef BUFSIZ
#define BUFSIZ 255 * 2

#ifdef _SEMAPHORE_
#define LOCK -1
#define UNLOCK 1
int semid;
#endif

void usage()
{
  fprintf(stdout, "./server <IP address> <port>\n");
  exit(EXIT_SUCCESS);
}

#ifdef _SEMAPHORE_
void semLockToggle(int semid, int semnum, int op)
{
  struct sembuf sops[1];

  sops[0].sem_num = semnum;
  sops[0].sem_op = op;
  sops[0].sem_flg = 0;

  if(semop(semid, sops, 1) < 0)
    {
      perror("semop()");
      exit(EXIT_FAILURE);
    }
}
#endif

void reply(void *fd)
{
  int conn_fd = *((int *)fd), rsize, fifodes;
  char buf[BUFSIZ];
  fd_set fds, readfds;
  struct timeval tv;
  int maxfd;

  fifodes = open("my_fifo",O_WRONLY);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&readfds);
  FD_SET(conn_fd, &readfds);
  FD_SET(fifodes, &readfds);

  maxfd = conn_fd;
  if(conn_fd < fifodes)
    maxfd = fifodes;

  while(1)
    {
      memcpy(&fds, &readfds, sizeof(fd_set));
      select(maxfd +1, &fds, NULL, NULL, &tv);

    if(FD_ISSET(conn_fd, &fds))
      {
	puts("recv");
	if((rsize = recv(conn_fd, buf, BUFSIZ, 0)) < 0)
	  {
	    perror("recv()");
	    exit(EXIT_FAILURE);
	  }
	
	else if(rsize == 0)
	  {
	    break;
	  }
	
	else
	  {
	    printf("%s\n", buf);
	    goodbyeReturn(buf);
	    if(strcmp(buf, "exit") == 0)
	      break;
	    
#ifdef _SEMAPHORE_
	    semLockToggle(semid, 1, UNLOCK);
	    semLockToggle(semid, 0, LOCK);
#endif
	    //	    write(conn_fd, buf, rsize);
	    write(fifodes, buf, rsize);
#ifdef _SEMAPHORE_
	    semLockToggle(semid, 1, LOCK);
	    semLockToggle(semid, 0, UNLOCK);
#endif
	  }
      }

    else if(FD_ISSET(fifodes, &fds))
      {
	read(fifodes,buf,BUFSIZ);
	printf("ponyo:%s\n", buf);
      }
    }
  
  if(close(conn_fd) < 0)
    {
      perror("close");
      exit(EXIT_FAILURE);
    }
  
  printf("close %d\n", conn_fd);
}

int main(int argc, char **argv)
{
  int server_sockfd, client_sockfd;
  struct sockaddr_in server_addr, client_addr;
  int server_addrlen, client_addrlen;
  pthread_t worker;
  int opt;

#ifdef _SEMAPHORE_
  union semun{ int val; struct semid_ds *buf; unsigned short *array; } c_arg;
  unsigned short vals[2] = { 0, 0 };
#endif
 
  if(argc != 3)
    usage();

  bzero((char *)&server_addr, sizeof(server_addr));
  mkfifo("my_fifo",0777);

#ifdef _SEMAPHORE_
  if((semid = semget(IPC_PRIVATE, SOMAXCONN, 0600)) < 0)
    {
      perror("semget()");
      exit(EXIT_FAILURE);
    }

  c_arg.array = vals;
  if (semctl(semid, 0, SETALL, c_arg) < 0)
    {
      perror("semctl()");
      exit(EXIT_FAILURE);
    }
#endif

  puts("socket");
  /**
   * int
   * socket(int domain, int type, int protocol);
   */
  if((server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addrlen = sizeof(server_addr);

  puts("setsockopt");
  /**
   * int
   * setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len); 
   */
  opt = 1;
  if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt)) < 0)
    {
      perror("setsockopt()");
      exit(EXIT_FAILURE);
    }

  puts("bind");
  /**
   * int
   * bind(int socket, const struct sockaddr *address, socklen_t address_len); 
   */
  if(bind(server_sockfd, (const struct sockaddr *)&server_addr, (socklen_t)server_addrlen) < 0)
    {
      perror("bind()");
      exit(EXIT_FAILURE);
    }

  puts("listen");
  /**
   * int
   * listen(int socket, int backlog);
   */
  if(listen(server_sockfd, SOMAXCONN) < 0)
    {
      perror("listen()");
      exit(EXIT_FAILURE);
    }  

  while(1)
    {
      puts("accept");
      /**
       * int
       * accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
       */
      if((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen)) < 0)
	{
	  perror("accept()");
	  exit(EXIT_FAILURE);
	}

      puts("pthread_create");
      /**
       * int
       * pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void *), void *restrict arg);
       */
      if(pthread_create(&worker, NULL, (void *)reply, (void *)&client_sockfd) != 0)
	{
	  perror("pthread_create()");
	  exit(EXIT_FAILURE);
	}
      
      puts("pthread_detach");
      /**
       * int
       * pthread_detach(pthread_t thread);
       */
      if(pthread_detach(worker) < 0)
	{
	  perror("pthread_detach()");
	  exit(EXIT_FAILURE);
	}
    }

  if(close(server_sockfd) < 0)
    {
      perror("close()");
      exit(EXIT_FAILURE);
    }

#ifdef _SEMAPHORE_
  if(semctl(semid, 0, IPC_RMID, c_arg) < 0)
    {
      perror("semctl()");
      exit(EXIT_FAILURE);
    }
#endif

  return 0;
}
