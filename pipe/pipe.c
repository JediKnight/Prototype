#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/select.h>
#include <sys/file.h>

#define LOCK -1
#define UNLOCK 1
#define getStatus(a) (a > 0) ? "UNLOCK" : "LOCK"
#define CHILDNM 4

/**
 * semaphore 
 */
union semunc { int val; struct semid_ds *buf; unsigned short *array; } arg;
void semLockToggle(int p_semid, int p_semnum, int p_op)
{
  struct sembuf sops[1];

  sops[0].sem_num = p_semnum;
  sops[0].sem_op = p_op;
  sops[0].sem_flg = 0;

  if(semop(p_semid, sops, 1) == -1)
    { perror("semop()"); exit(EXIT_FAILURE); }
}

int semSetting()
{
  int id;
  unsigned short vals[2] = { 0, 0 };

  if((id = semget(IPC_PRIVATE, CHILDNM, 0600)) == -1)
    { perror("semget()"); return -1; }

  arg.array = vals;
  if(semctl(id, 0, SETALL, arg) == -1)
    { perror("SETALL semctl()"); return -1; }

  return id;
}

int semClose(int id)
{
  if(semctl(id, 0, IPC_RMID, arg) == -1)
    { perror("IPC_RMID semctl()"); return -1; }

  return 0;
}
/* semaphore end */


/**
 * fifo
 */
#define FIFO_PATH "fifo"
#define PERMITTION 0777
int fifo()
{
  int f;
  mkfifo(FIFO_PATH, PERMITTION);
  f = open(FIFO_PATH, O_RDWR|O_NONBLOCK|O_CREAT);
  return f;
}
/* fifo end */


/**
 * fork
 */
enum { R = 0, W };
void child(int childnm, int semid)
{
  int fifodes[2];
  int i;
  char buf[255];

  fifodes[R] = open("fifo", O_RDONLY|O_NONBLOCK|O_CREAT);
  fifodes[W] = open("fifo", O_WRONLY|O_NONBLOCK|O_CREAT);

  if(childnm == 0)
    {
      for(i = 0; i < CHILDNM; i++)
	{
	  printf("process%d start\n", childnm);
	  memset(buf, '\0', 255);
	  sprintf(buf, "child:%d", getpid());
	  write(fifodes[W], buf, 255);
	
	  if(i == 0)
	    {
	      printf("setting:%d, child:%d, status:%s\n", childnm, 2, getStatus(LOCK));
	      printf("setting:%d, child:%d, status:%s\n", childnm, 1, getStatus(LOCK));
	      printf("setting:%d, child:%d, status:%s\n", childnm, 0, getStatus(UNLOCK));
	      printf("-------------------------------\n");
	      semLockToggle(semid, 2, LOCK);
	      semLockToggle(semid, 1, LOCK);
	      semLockToggle(semid, 0, UNLOCK);
	    }

	  else if(i == 1)
	    {
	      printf("setting:%d, child:%d, status:%s\n", childnm, 2, getStatus(LOCK));
	      printf("setting:%d, child:%d, status:%s\n", childnm, 1, getStatus(LOCK));
	      printf("setting:%d, child:%d, status:%s\n", childnm, 0, getStatus(UNLOCK));
	      printf("-------------------------------\n");
	      semLockToggle(semid, 2, LOCK);
	      semLockToggle(semid, 1, LOCK);
	      semLockToggle(semid, 0, UNLOCK);
	    }
	}
    }

  if(childnm == 1)
    {
      printf("process%d start\n", childnm);
      printf("setting:%d, child:%d, status:%s\n", childnm, 2, getStatus(LOCK));
      printf("setting:%d, child:%d, status:%s\n", childnm, 1, getStatus(UNLOCK));
      printf("setting:%d, child:%d, status:%s\n", childnm, 0, getStatus(LOCK));
      printf("-------------------------------\n");
      semLockToggle(semid, 2, UNLOCK);
      semLockToggle(semid, 1, LOCK);
      semLockToggle(semid, 0, LOCK);

      printf("get message:");
      read(fifodes[R], buf, 255);
      printf("%s\n", buf);
    }

  if(childnm == 2)
    {
      printf("process%d start\n", childnm);
      printf("setting:%d, child:%d, status:%s\n", childnm, 2, getStatus(UNLOCK));
      printf("setting:%d, child:%d, status:%s\n", childnm, 1, getStatus(LOCK));
      printf("setting:%d, child:%d, status:%s\n", childnm, 0, getStatus(LOCK));
      printf("-------------------------------\n");
      semLockToggle(semid, 2, LOCK);
      semLockToggle(semid, 1, LOCK);
      semLockToggle(semid, 0, UNLOCK);

      printf("process%d start\n", childnm);
      printf("get message:");
      read(fifodes[R], buf, 255);
      printf("%s\n", buf);
    }

  printf("process%d end\n", childnm +1);
  exit(EXIT_SUCCESS);
}

void parent(int cnt)
{
  int i;
  for(i = 0; i < cnt; i++)
    wait(NULL);
}
/* fork end */


int main(int argc, char** argv)
{
  pid_t pid;
  int semid, i;
  char buf[255];

  if((semid = semSetting()) == -1)
    { fprintf(stderr, "semSetting()"); exit(EXIT_FAILURE); }

  for(i = 0; i < CHILDNM; i++)
    {
      if((pid = fork()) == -1)
	{
	  return -1;
	}

      else if(pid == 0)
	{ child(i, semid); }

      else
	{
	  parent(i);
	}
    }

  semClose(semid);
  return 0;
}
