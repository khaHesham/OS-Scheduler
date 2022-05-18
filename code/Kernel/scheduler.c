#include "../Defs.h"

#include "../DS/Data_structures.h"

#include "../Clock/clk_interface.h"

#include <stdio.h>

#include <stdlib.h>

#include <math.h>





ALGORITHM algorithm;

int NP; // Total number of processes

PCB **ProcessTable;

void *ready_queue;



// The state of the current process

processData *current_process;

int current_process_id = 0;

int current_process_start_time;

// To be used in computing cpu utilization

int initial_start_time;

int last_finish_time;

int Total_running_time;

// To be used in computing .perf file

int Total_Waiting = 0;

int Total_WTA = 0;

int *WTA_arr; // for std deviation

// For termintion condition(finished_processes == NP)

int finished_processes = 0;

int quantum = 0; // running quantum in case of RR





int msgqid;               ////msgqueue between schedualer and generator

 msgbuff message;

int pid;                 ///id of the actual running process



// signal handlers



void ProcessFinished(int signum);

void ProcessArrived(int signum);

void SwitchContext(int previous_id);



// print data

void Performance_file();

void logfile( char state,int TA ,float WTA) ;              



void CreateProcess(int remainingtime)

{

     pid = fork();

    if (pid == 0)

        execl("process.out", "./process.out", remainingtime, NULL);

}



/*Insert a process into ready_queue ellegantly*/

void EnqueueProcess(void *ready_queue, processData *process)

{

    if (algorithm == SRTN || algorithm == HPF)

        push((PriorityQueue *)ready_queue, process);

    else if (algorithm == RR)

        enqueue((Queue *)ready_queue, process);

}



/*Fetch a process from ready_queue ellegantly(Don't forget to use free())*/

processData *DequeueProcess(void *ready_queue)

{

    if (algorithm == SRTN || algorithm == HPF)

        return pop((PriorityQueue *)ready_queue);

    return dequeue((Queue *)ready_queue);

}



void *CreateReadyQueue()

{

    switch (algorithm)

    {

    case SRTN:

        return PriorityQueue_init(SRTN);

    case HPF:

        return PriorityQueue_init(HPF);

    case RR:

        return Queue_init();

    }

}



int get_current_remaining()

{

    int actual_running = getClk() - current_process_start_time;

    return current_process->remainingtime - actual_running;

}



int main(int argc, char *argv[])

{

    initClk();

    signal(SIGUSR1, ProcessFinished);

    signal(SIGUSR2, ProcessArrived);



    if (argc != 3)

        return (1);



    algorithm = atoi(argv[1]);

    NP = atoi(argv[2]);

    quantum = atoi(argv[3]);



    ProcessTable = malloc(NP * sizeof(PCB*));

    WTA_arr = malloc(NP * sizeof(int));



    // It might be a little confusing why the ready queue is of void type

    // but you can think of it as trying to use generic types in C

    // it must be freed at the end of the scheduler as it's dynamically allocated

    ready_queue = CreateReadyQueue();

    // connect to msgq(which is created by the process generator)

    msgqid = msgget(MSGQKEY, 0444);



    if (msgqid == -1)    //error

        return (1);



    // There should be a while loop where the scheduler sleeps untill it receives

    // some signal but I'm not sure yet how to implement it

    // destroyClk(true);  moved to handler

}



void ProcessFinished(int signum)

{

    int crr_remaining_time = get_current_remaining();

    if (crr_remaining_time == 0) // always true in HPF SJF FCFS

    {                                                                                      ////process finished

         

        int TA = getClk() - current_process->arrivaltime;

       ProcessTable[current_process_id]->waiting_time=TA - current_process->runningtime;

        float WTA = (float) TA / current_process->runningtime;

          logfile('f',TA,WTA);                              ///finished state

            ProcessTable[current_process_id] = NULL;            ////to remove the process from process table

        // make the neccesary (print/file) for the log file   (schaduler.log)

                                                                                

        Total_Waiting +=  ProcessTable[current_process_id]->waiting_time;

        Total_WTA += WTA;

        Total_running_time += current_process->runningtime;

        WTA_arr[current_process_id] = WTA;

        finished_processes++;

        if (finished_processes == NP)

        {

            Performance_file(); // schaduler.perf file

            exit(0);

        }

    }

    else // in case of RR and SRTN to restore process in ready_queue

    {    

       

        EnqueueProcess(ready_queue, current_process);

    }

    // schedule new process

    SwitchContext(current_process_id);

}



void ProcessArrived(int signum)

{

    // TODO implement the second handler

    msgrcv(msgqid, &message, sizeof(message.process), 0 , !IPC_NOWAIT);  

     EnqueueProcess(ready_queue, &(message.process));

     

     ////insert pcb of the process to the process table

    PCB  * p= malloc(sizeof(PCB));  /// check

     p->waiting_time=0;

     p->execution_time=0;

     p->remaining_time = (message.process).runningtime;   /////check ?????  tmm keda



       ///insert it in the ProcessTable how in which position???  

   ProcessTable[(message.process).id] = p;    ///discuess????????

   

   ///if the algorithm = SRTN



 if(algorithm == SRTN)

        {

            if((message.process).runningtime < get_current_remaining())

                kill(SIGINT,pid);                                      //////make pid of the process global to see it???

                SwitchContext(current_process_id);

        }

}



void SwitchContext(int previous_id) // we need to choose the place which will contain all the updated numbers (ProcessTable or ProcessData)

{

    if (ProcessTable[previous_id]->remaining_time != 0) // save state of old_id

    {                                                                                    /////process stopped??

        ProcessTable[previous_id]->remaining_time = get_current_remaining();

        current_process->remainingtime=get_current_remaining();          ///update remaining also in processdata

        //update remaining process data

        //calculate waiting time 

        ProcessTable[previous_id]->execution_time += current_process->runningtime;

        ProcessTable[previous_id]->waiting_time=getClk()-current_process->arrivaltime-(current_process->runningtime-current_process->remainingtime);

        ProcessTable[previous_id]->state = WAITING;

        logfile('d',0,0);

        // make the neccesary (print/file) for the log file   (schaduler.log)

    }

    current_process = DequeueProcess(ready_queue);                             ////process started

    current_process_id = current_process->id;

    current_process_start_time = getClk();

    ProcessTable[current_process_id]->state = RUNNING;



    if(current_process->runningtime == current_process->remainingtime)

    {

            ProcessTable[current_process_id]->waiting_time=getClk()-current_process->arrivaltime;    ///process started 

            logfile('s',0,0);                                   

    }

            else {

                ProcessTable[current_process_id]->waiting_time=getClk()-current_process->arrivaltime-(current_process->runningtime-current_process->remainingtime); ///process resumed

                logfile('r',0,0);                 

            }



   /////check if the process started or resumed by (running == remaining --> started)    //nada



    // make the neccesary (print/file) for the log file   (schaduler.log)

    if (algorithm == RR && ProcessTable[current_process_id]->remaining_time > quantum)

                          CreateProcess(quantum);

    else                       

       CreateProcess(ProcessTable[current_process_id]->remaining_time);

}



void Performance_file()

{

    float sum=0.0,mean=0.0,SD=0.0;

    FILE * myfile = fopen("/home/os/osproject/allcode/performance file.txt","w");

 if(myfile==NULL)

    {perror("unable to open the file");

 exit(0);

    }

    //cpu utlization

     fprintf(myfile, "cpu utilization =%.2f\n", ((float)Total_running_time/(last_finish_time - initial_start_time) )*100); 

     //Avg WTA

    fprintf(myfile, "Avg WTA = %.2f\n", (float)Total_WTA/NP); 

    //Avg Waiting

    fprintf(myfile, "Avg Waiting = %.2f\n", (float)Total_Waiting/NP); 

 



    // Calculating Mean

    for (int i = 0; i < NP; i++)

    sum += WTA_arr[i];

    mean = sum / NP;

    

    // Calculating Standard Deviation

    sum = 0.0;

    for (int i = 0; i < NP; i++)

    sum += (WTA_arr[i] - mean) * (WTA_arr[i] - mean);

    SD = sqrt(sum / NP);



    printf("Std WTA %.2f\n", SD);

    // TODO

}

void logfile( char state,int TA ,float WTA)

{



 FILE * myfile = fopen("/home/os/osproject/allcode/logfile.txt","w");

 if(myfile==NULL)

    {perror("unable to open the file");

 exit(0);

    }



fprintf(myfile , "#At time X process Y state arr W total Z remain Y wait K"); 



switch(state)

{

case 's': fprintf(myfile , "At time %d process %d started  arr  %d  total %d  remain %d  wait %d \n",getClk(),current_process_id,current_process->arrivaltime,current_process->runningtime,ProcessTable[current_process_id]->remaining_time,ProcessTable[current_process_id]->waiting_time);  ///started

            break;



case 'd':   fprintf(myfile , "At time %d process %d started  arr  %d  total %d  remain %d  wait %d \n",getClk(),current_process_id,current_process->arrivaltime,current_process->runningtime,ProcessTable[current_process_id]->remaining_time,ProcessTable[current_process_id]->waiting_time);   ///stopped

              break;



case 'f':     fprintf(myfile , "At time %d process %d finished  arr  %d  total %d  remain %d  wait %d  TA  %d  WTA  %f  \n",getClk(),current_process_id,current_process->arrivaltime,current_process->runningtime,ProcessTable[current_process_id]->remaining_time,ProcessTable[current_process_id]->waiting_time,TA,WTA);   //finished

               break;



case 'r':         fprintf(myfile , "At time %d process %d started  arr  %d  total %d  remain %d  wait %d \n",getClk(),current_process_id,current_process->arrivaltime,current_process->runningtime,ProcessTable[current_process_id]->remaining_time,ProcessTable[current_process_id]->waiting_time);    ///resumed

                  break;

}

}