#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>

#define LOCK -1
#define UNLOCK 1

void semLockToggle(int p_semid, int p_semnum, int p_op)
{
  struct sembuf sops[1];

  sops[0].sem_num = p_semnum;
  sops[0].sem_op = p_op;
  sops[0].sem_flg = 0;

  if(semop(p_semid, sops, 1) == -1)
    {
      perror("MySemop ");
      exit(EXIT_FAILURE);
    }
}

void childp(int index, int semid)
{
  int i;

  if(index == 0) 
    sleep(3);

  printf("process:%d\n", index +1);
  for(i = 0; i < 10; i++)
    printf("%d:%d\n", i, getpid());

  if(index == 0)
    {
      semLockToggle(semid, 1, UNLOCK);
      semLockToggle(semid, 0, LOCK);
    }
  
  else
    {
      semLockToggle(semid, 0, UNLOCK);
      semLockToggle(semid, 1, LOCK);
    }

  exit(EXIT_SUCCESS);
}

void parentp()
{
  int status;
  wait(&status);
}

int main(int argc, char** argv)
{
  pid_t pid[2];
  int semid, i;
  union semun { int val; struct semid_ds *buf; unsigned short *array; } ctl_arg;
  unsigned short vals[2] = { 0, 0 };

  if((semid = semget(IPC_PRIVATE, 2, 0600)) == -1)
    { perror("main : semget "); exit(EXIT_FAILURE); }

  ctl_arg.array = vals;
  if(semctl(semid, 0, SETALL, ctl_arg) == -1)
    { perror("main : semctl "); exit(EXIT_FAILURE); }

  for(i = 0; i < 2; i++)
    {
      if((pid[i] = fork()) == -1)
	{
	  return -1;
	}

      else if(pid[i] == 0)
	{
	  childp(i, semid);
	}

      else
	{
	  parentp();
	}
    }

  if(semctl(semid, 0, IPC_RMID, ctl_arg) == -1)
    {
      perror("main : semctl ");
      exit(EXIT_FAILURE);
    }
    
  return 0;
}
