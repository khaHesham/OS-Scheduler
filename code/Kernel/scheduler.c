#include "../Defs.h"
#include "../DS/Data_structures.h"
#include "../Clock/clk_interface.h"

ALGORITHM algorithm;
int NP; // Total number of processes
PCB *ProcessTable;
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

// signal handlers
void ProcessFinished(int signum);
void ProcessArrived(int signum);
void SwitchContext(int previous_id);
void Performance_file();

void CreateProcess(int remainingtime)
{
    int pid = fork();
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
    int actual_running = get_clock() - current_process_start_time;
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

    ProcessTable = malloc(NP * sizeof(PCB));
    WTA_arr = malloc(NP * sizeof(int));

    // It might be a little confusing why the ready queue is of void type
    // but you can think of it as trying to use generic types in C
    // it must be freed at the end of the scheduler as it's dynamically allocated
    ready_queue = CreateReadyQueue();
    // connect to msgq(which is created by the process generator)
    int msgqid = msgget(MSGQKEY, 0444);

    if (msgqid == -1)
        return (1);

    // There should be a while loop where the scheduler sleeps untill it receives
    // some signal but I'm not sure yet how to implement it
    // destroyClk(true);  moved to handler
}

void ProcessFinished(int signum)
{
    int crr_remaining_time = get_current_remaining();
    if (crr_remaining_time == 0) // always true in HPF SJF FCFS
    {
        // ProcessTable[current_process_id] = NULL;
        int TA = get_clock() - current_process->arrivaltime;
        int waiting_time = TA - current_process->runningtime;
        int WTA = TA / current_process->runningtime;

        // make the neccesary (print/file) for the log file   (schaduler.log)

        Total_Waiting += waiting_time;
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
}

void SwitchContext(int previous_id) // we need to choose the place which will contain all the updated numbers (ProcessTable or ProcessData)
{
    if (ProcessTable[previous_id].remaining_time != 0) // save state of old_id
    {
        ProcessTable[previous_id].remaining_time = get_current_remaining();
        current_process->runningtime = get_clock() - current_process_start_time;
        ProcessTable[previous_id].execution_time += current_process->runningtime;
        ProcessTable[previous_id].state = WAITING;
        // make the neccesary (print/file) for the log file   (schaduler.log)
    }
    current_process = DequeueProcess(ready_queue);
    current_process_id = current_process->id;
    current_process_start_time = get_clock();
    ProcessTable[current_process_id].state = RUNNING;

    // make the neccesary (print/file) for the log file   (schaduler.log)
    if (algorithm == RR)

    {
        int pid = fork();
        if (pid == 0)
            execl("process", "./process", quantum, NULL);
    }

    else // send the remaining time to the process

    {

        int pid = fork();
        if (pid == 0)
            execl("process", "./process", current_process->remainingtime, NULL);
    }
}

void Performance_file()
{
    // TODO
}

// Some functions dealing with the .log file