#include "../Defs.h"
#include "../DS/Data_structures.h"
#include "../Clock/clk_interface.h"


ALGORITHM algorithm;
int NP;  //Total number of processes

//The state of the current process
processData current_process;
int current_process_id  = 0;
int current_process_start_time;

//To be used in computing cpu utilization
int initial_start_time;
int last_finish_time;
int Total_running_time;

//To be used in computing .perf file
int Total_Waiting = 0;
int Total_WTA = 0;
int *WTA_arr; //for std deviation

//For termintion condition(finished_processes == NP)
int finished_processes = 0;

//signal handlers
void ProcessFinished(int signum);
void ProcessArrived(int signum);

void SwitchContext(int previous_id);
void Performance_file();


void CreateProcess(int remainingtime)
{
    int pid = fork();
    if(pid == 0)
        execl("process.out", "./process.out", remainingtime, NULL);
}

/*Insert a process into ready_queue ellegantly*/
void EnqueueProcess(void* ready_queue, processData* process)
{
    if(algorithm == SRTN || algorithm == HPF)
        push((PriorityQueue*)ready_queue, process);

    else if(algorithm == RR)
        enqueue((Queue*)ready_queue, process); 
}

/*Fetch a process from ready_queue ellegantly(Don't forget to use free())*/
processData* DequeueProcess(void* ready_queue)
{
    if(algorithm == SRTN || algorithm == HPF)
        return pop((PriorityQueue*)ready_queue);

    return dequeue((Queue*)ready_queue); 
}

void* CreateReadyQueue()
{
    switch(algorithm)
    {
        case SRTN: 
            return PriorityQueue_init(SRTN);
        case HPF: 
            return PriorityQueue_init(HPF);
        case RR: 
            return Queue_init();
    }
}


int main(int argc, char * argv[])
{
    initClk();

    signal(SIGUSR1, ProcessFinished);
    signal(SIGUSR2, ProcessArrived);

    
    if(argc != 3)
        return(1);
    
    algorithm = atoi(argv[1]);
    int NP = atoi(argv[2]);

    PCB* ProcessTable = malloc(NP* sizeof(PCB));
    WTA_arr = malloc(NP* sizeof(int));

    //It might be a little confusing why the ready queue is of void type
    //but you can think of it as trying to use generic types in C
    //it must be freed at the end of the scheduler as it's dynamically allocated
    void *ready_queue = CreateReadyQueue();


    //connect to msgq(which is created by the process generator)
    int msgqid = msgget(MSGQKEY, 0444);
    if(msgqid == -1)
        return(1);
    
    //There should be a while loop where the scheduler sleeps untill it receives 
    //some signal but I'm not sure yet how to implement it

    //destroyClk(true);  moved to handler
}



void ProcessFinished(int signum)
{
    //TODO implement the first handler
}
void ProcessArrived(int signum)
{
    //TODO implement the second handler
}

void SwitchContext(int previous_id)
{
    //TODO
}

void Performance_file()
{
    //TODO
}

//Some functions dealing with the .log file