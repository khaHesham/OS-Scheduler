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

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
 processData process;
     Queue * processes;
     processes = Queue_init();

 FILE * myfile = fopen(argv[1],"r");
 if(myfile==NULL)
 {perror("unable to open the file");
 exit(1);
 }
  char line [100];
  char * data;
  int number=0;   
  int test;
  int algo;
  int totalnumprocesses=0;
 int quantum;
 ALGORITHM algorithm;
   int IDS [2];
  fgets(line,sizeof(line),myfile);      ///for ignoring the first line in the file (#id arrivaltime ....)

   while(fgets(line,sizeof(line),myfile))
   {    
        totalnumprocesses++;
      data= strtok(line," ");
            while(data!=NULL)
      {  
          printf(" data is %s \n", data);
           switch(number)
           {
               case 0:
                     process.id=atoi(data);
                     break;
               case 1:
                     process.arrivaltime=atoi(data);
                    break;
               case 2:
                  
                       process.runningtime =atoi(data);
                    process.remainingtime=atoi(data);
                   break;
               case 3:
                       process.priority=atoi(data);
                    break;      
           }
    enqueue(processes,&process);
      //  test=atoi(data);
         // process->priority=atoi(data);
         //printf("iam priority %d \n ", process.priority);
        //printf("iam test %d \n ",test); 
          data = strtok(NULL," ");
       number++;
      }
      number=0;
   }
fclose(myfile);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
      printf("enter the number of algortihm u want : \n \n");
      printf("(1_Roundrobin , 2_SRTN , 3_HPF) \n ");
      scanf("%d", &algo);
      if(algo==1)
  {    printf("enter the quntum :\n");
       scanf("%d", &quantum);}

       switch(algo)
       {
           case 1:
                      algorithm = RR;
                 break;
           case 2:    algorithm =SRTN;
                 break;
           case 3:    algorithm =HPF;    
                 break;
       }
     // printf("the algorithm is %d ",algorithm);
    // 3. Initiate and create the scheduler and clock processes.
    for(int i=0 ;i<2; i++)
  {
       int pid = fork();
       IDS[i]=pid;                   ////store the scheduler and clock IDS 
     
     if(pid==-1)
perror("error in fork");
  else if(pid == 0)                                          ///the child
    {    if(i==0)  
            execl("./clk.out","./clk.out", NULL);       ///for intiating clock
         //   else  execl("../scheduler.out", "./scheduler.out",algorithm,totalnumprocesses,quantum,NULL);            ///for initiating the schedualer
    }
  }
    // 4. Use this function after creating the clock process to initialize clock
       initClk();
    // To get time use this
        int x = getClk();
         printf("current time is %d \n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    //create msgbuff struct
    struct msgbuff{
  struct  processData process;
};
    //create msgq
    processData next_process;
    int current_time=-1;
    key_t key_id;
    int send_val;
    struct msgbuff message;
    key_id= ftok("keyfile", 1);
    int msqid = msgget(key_id, IPC_CREAT | 0644);
    if (msqid == -1)
     { perror("Error in create message queue to Scheduler\n");
       exit(-1);
     }

    while(!processes.empty())
    {
        current_time =getClk();

        
            next_process =processes.peek();
            if(next_process.arriva_time != current_time)
                break;
            processes.dequeue();
            //send to scheduler
            send_val = msgsnd(msqid, &message, sizeof(message.process), !IPC_NOWAIT);
            if (send_val == -1)
            perror("Process Generator: Errror in sending process  to Scheduler");
            //send to scheduler
    }

    waitpid( IDS [1]);
    clearResources();//destroyclock(true)
}
  return 0;
}

// 7. Clear clock resources
  //1  destroyClk(true);

void clearResources()
{
    //TODO Clears all resources in case of interruption
    msgctl(msqid, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    exit(0);
}