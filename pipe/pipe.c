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
#define CHILDNM 2

/**
 * semaphore 
 */
union semunc { int val; struct semid_ds *buf; unsigned short *array; } arg;
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

#define setSemopStatus(a, b) (a == b) ? UNLOCK : LOCK
int semLockEx(int semid, int id)
{
  int i;
  for(i = 0; i < CHILDNM; i++)
    {
      struct sembuf sops[1];
      sops[0].sem_num = i;
      sops[0].sem_op = setSemopStatus(id, i);
      sops[0].sem_flg = 0;
      if(semop(semid, sops, 1) == -1)
	{ perror("semLockAll semop()"); return -1; }
    }
  return 0;
}

int semUnlock(int semid, int semnum)
{
  struct sembuf sops[1];
  sops[0].sem_num = semnum;
  sops[0].sem_op = UNLOCK;
  sops[0].sem_flg = 0;

  if(semop(semid, sops, 1) == -1)
    { perror("semUnlock semop()"); return -1; }
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
void child(int semid, int semnm)
{
  int fifodes[2];
  int i;
  char buf[255];

  fifodes[R] = open("fifo", O_RDONLY|O_NONBLOCK|O_CREAT);
  fifodes[W] = open("fifo", O_WRONLY|O_NONBLOCK|O_CREAT);

  if(semnm == 0)
    {
      printf("process%d start\n", semnm);
      printf("process%d end\n", semnm);
      semLockEx(semid, 1);
    }

  else if(semnm == 1)
    {
      semLockEx(semid, 0);
      printf("process%d start\n", semnm);
      printf("process%d end\n", semnm);
    }

  else if(semnm == 2)
    {
      semLockEx(semid, 0);
      printf("process%d start\n", semnm);
      printf("process%d end\n", semnm);
    }

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
	{ child(semid, i); }

      else
	{
	  parent(i);
	}
    }

  semClose(semid);
  return 0;
}
