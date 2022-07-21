#include <stdio.h>

#include "../Defs.h"

#include "../Clock/clk_interface.h"

#include "../DS/Data_structures.h"

#include <string.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/msg.h>

#include <stdlib.h>

#include <unistd.h>

#include <math.h>

void clearResources();

int msqid;

union Semun
{
  int val;               /* value for SETVAL */
  struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
  ushort *array;         /* array for GETALL & SETALL */
  struct seminfo *__buf; /* buffer for IPC_INFO */
  void *__pad;
};

void down(int sem)
{
  struct sembuf p_op;

  p_op.sem_num = 0;
  p_op.sem_op = -1;
  p_op.sem_flg = !IPC_NOWAIT;

  if (semop(sem, &p_op, 1) == -1)
  {
    perror("Error in down()");
    exit(-1);
  }
}

void up(int sem)
{
  struct sembuf v_op;

  v_op.sem_num = 0;
  v_op.sem_op = 1;
  v_op.sem_flg = !IPC_NOWAIT;

  if (semop(sem, &v_op, 1) == -1)
  {
    perror("Error in up()");
    exit(-1);
  }
}

int main(int argc, char *argv[])

{

  union Semun semun;

  int sem = semget(SEMKEY, 1, 0666 | IPC_CREAT);

  signal(SIGINT, clearResources);
  if (sem == -1)
  {
    perror("Error in create sem");
    exit(-1);
  }

  semun.val = 1; /* initial value of the semaphore, Binary semaphore */
  if (semctl(sem, 0, SETVAL, semun) == -1)
  {
    perror("Error in semctl");
    exit(-1);
  }

  // 1. Read the input files.

  Queue *processes;
  processes = Queue_init();

  FILE *myfile = fopen("Test/processes.txt", "r");

  if (myfile == NULL)
  {
    perror("unable to open the file");

    exit(1);
  }

  char line[100];

  char *data;

  int number = 0;

  int test;

  char algo[4];

  int totalnumprocesses = 0;

  char quantum[2];
  char tp[20];
  ALGORITHM algorithm;

  int IDS[2];

  char c;

  fgets(line, sizeof(line), myfile); /// for ignoring the first line in the file (#id arrivaltime ....)
                                     // int c = getc(myfile);
  while (true)
  {
    processData *process = malloc(sizeof(processData));
    c = fscanf(myfile, "%d%d%d%d%d", &(process->id), &(process->arrivaltime), &(process->runningtime), &(process->priority), &(process->memsize));
    if (c == EOF)
      break;
    process->remainingtime = process->runningtime;
    enqueue(processes, process);
    totalnumprocesses++;
  }



  fclose(myfile);
  processData *test1;

  printf("enter the number of algortihm u want : \n \n");
  printf("(0_Roundrobin , 1_SRTN , 2_HPF) \n ");

  scanf("%s", algo);
  int y = atoi(algo);
  strcpy(quantum, "0");
  if (y == 0)
  {
    printf("enter the quntum :\n");

    scanf("%s", quantum);
  }


  sprintf(tp, "%d", totalnumprocesses);

  int pid = fork();
  IDS[0] = pid; ////store the scheduler and clock IDS
  if (pid == -1)
    perror("error in fork");
  else if (pid == 0)                       /// the child
    execl("./clk.out", "./clk.out", NULL); /// for intiating clock
  pid = fork();
  IDS[1] = pid; ////store the scheduler and clock IDS
  if (pid == -1)
    perror("error in fork");
  else if (pid == 0)
  {
    printf("creating schedauler nw \n");
    printf("schadualer id is %d \n", getpid());
    execl("./scheduler.out", "./scheduler.out", algo, tp, quantum, NULL);
  } /// for initiating the schedualer
  // 4. Use this function after creating the clock process to initialize clock

  initClk();


  int x = getClk();
  printf("current time is %d \n", x);

  // TODO Generation Main Loop

  // 5. Create a data structure for processes and provide it with its parameters.

  // 6. Send the information to the scheduler at the appropriate time.

  // create msgbuff struct

  // create msgq

  processData *next_process;

  int current_time;

  int send_val;

  struct msgbuff message;

  message.mtype = 1;

  msqid = msgget(MSGQKEY, IPC_CREAT | 0666);

  if (msqid == -1)
  {
    fflush(stdout);
    perror("Error in create message queue to Scheduler\n");
    exit(-1);
  }

  while (!isEmpty(processes))
  {

    next_process = front(processes);
    current_time = getClk();
    //  printf("next process arrival time %d and currenttime %d \n",next_process->arrivaltime,getClk());
    if (next_process->arrivaltime <= current_time)
    {
      message.process = *(dequeue(processes));
      printf("messge send process id %d its arrvial time %d at current time %d \n", message.process.id, message.process.arrivaltime, current_time);
      down(sem);
      kill(IDS[1], SIGUSR2);
      fflush(stdout);
      send_val = msgsnd(msqid, &message, sizeof(message.process), !IPC_NOWAIT);
      if (send_val == -1)
      {
        fflush(stdout);
        perror("Process Generator: Errror in sending process  to Scheduler");
      }
    }
  }

  waitpid(IDS[1], 0, 0);

  clearResources(); // destroyclock(true)

  return 0;
}



void clearResources()
{
  msgctl(msqid, IPC_RMID, (struct msqid_ds *)0);
  destroyClk(true);
}
