#include "../Defs.h"
#include "../DS/Data_structures.h"
#include "../Clock/clk_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../mmry/memory.h"

ALGORITHM algorithm;
int NP; // Total number of processes
PCB **ProcessTable;
void *ready_queue;
void *waiting_queue;

bool is_any_process_running = false;

processData *current_process;
int current_process_id = 0; // initially 0
int current_process_start_time;

int initial_start_time;
int last_finish_time;
int Total_running_time;

FILE *myfile;
// mmry
FILE *memFile;

int Total_Waiting = 0;
float Total_WTA = 0;
float *WTA_arr; // for std deviation

// For termintion condition(finished_processes == NP)

int finished_processes = 0;
int quantum = 0; // running quantum in case of RR

int index_of_WTA_arra = 0;

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

int msgqid; ////msgqueue between schedualer and generator
msgbuff message;
int pid; /// id of the actual running process
union Semun semun;
int sem;

// signal handlers

void ProcessFinished(int signum);
void ProcessArrived(int signum);
void SwitchContext(int previous_id);
// print data

void Performance_file();
void logfile(char state, int TA, float WTA);
void memoryFile(int state, int id, int start, int size);

void CreateProcess(int remainingtime)
{

    is_any_process_running = true;

    char buffer[10]; // mabsot?
    sprintf(buffer, "%d", remainingtime);

    pid = fork();
    if (pid == 0)
        execl("process.out", "./process.out", buffer, NULL);
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

int IS_EMPTY_QUEUE()
{

    if (algorithm == SRTN || algorithm == HPF)
        return Empty((PriorityQueue *)ready_queue);

    return isEmpty((Queue *)ready_queue);
}

int Size_Ready_Queue()
{
    if (algorithm == SRTN || algorithm == HPF)
        return size_prioQ((PriorityQueue *)ready_queue);

    else
        return size_queue((Queue *)ready_queue);
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

    initial_start_time = getClk();
    fflush(stdout);
    printf("intial is %d \n", initial_start_time);

    signal(SIGUSR1, ProcessFinished);
    signal(SIGUSR2, ProcessArrived);

    // mmry
    initMemory();
    memFile = fopen("Kernel/memory.txt", "w");
    fprintf(memFile, "#At time x allocated y bytes for process z from i to j\n");
    fflush(memFile);

    myfile = fopen("Kernel/logfile.txt", "w");

    if (myfile == NULL)
    {
        fflush(stdout);
        perror("unable to open the file");
    }
    fprintf(myfile, "#At time X process Y state arr W total Z remain Y wait K \n");

    sem = semget(SEMKEY, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        fflush(stdout);
        perror("Error in create sem");
    }

    if (argc != 4)
    {
        return (1);
    }

    algorithm = atoi(argv[1]);

    NP = atoi(argv[2]);

    quantum = atoi(argv[3]);

    ProcessTable = malloc((NP + 1) * sizeof(PCB *));

    WTA_arr = malloc(NP * sizeof(int));

    ready_queue = CreateReadyQueue();
    waiting_queue = CreateReadyQueue();

    // connect to msgq(which is created by the process generator)

    msgqid = msgget(MSGQKEY, IPC_CREAT | 0666);

    if (msgqid == -1) // error
    {
        fflush(stdout);
        perror("error yarab er7amnaaa!!");
        return (1);
    }

    while (1)
    {
        pause();
    }
}

void ProcessFinished(int signum)
{

    int crr_remaining_time = get_current_remaining();
    current_process->remainingtime = crr_remaining_time;
    ProcessTable[current_process_id]->remaining_time = crr_remaining_time;

    is_any_process_running = false;

    // process finished completely
    if (crr_remaining_time == 0) // always true in HPF
    {
        // mmry
        deallocate(ProcessTable[current_process_id]->index, ProcessTable[current_process_id]->memsize);
        memoryFile(1, current_process_id, ProcessTable[current_process_id]->index, ProcessTable[current_process_id]->memsize);

        int TA = getClk() - current_process->arrivaltime;
        ProcessTable[current_process_id]->waiting_time = TA - current_process->runningtime;
        float WTA = (float)TA / current_process->runningtime;

        Total_Waiting += ProcessTable[current_process_id]->waiting_time;
        Total_WTA += WTA;
        Total_running_time += current_process->runningtime;

        logfile('f', TA, WTA); /// finished state

        WTA_arr[index_of_WTA_arra] = WTA;
        index_of_WTA_arra++;

        finished_processes++;
        if (finished_processes == NP)
        {
            Performance_file(); // schaduler.perf file
            fclose(myfile);
            // mmry
            fclose(memFile);
            exit(0);
        }
    }

    else
    {

        processData *pro = malloc(sizeof(processData));
        pro->id = current_process->id;
        pro->runningtime = current_process->runningtime;
        pro->remainingtime = current_process->remainingtime;
        pro->priority = current_process->priority;
        pro->arrivaltime = current_process->arrivaltime;
        EnqueueProcess(ready_queue, pro);
    }

    SwitchContext(current_process_id);
    up(sem);
}

void ProcessArrived(int signum)
{
    int rcv = msgrcv(msgqid, &message, sizeof(message.process), 0, !IPC_NOWAIT);
    if (rcv == -1)
        perror("error in recieving the process in process arrived\n");

    processData *pro = malloc(sizeof(processData));
    pro->id = message.process.id;
    pro->runningtime = message.process.runningtime;
    pro->remainingtime = message.process.remainingtime;
    pro->priority = message.process.priority;
    pro->arrivaltime = message.process.arrivaltime;
    pro->memsize = message.process.memsize;
    EnqueueProcess(ready_queue, pro);

    PCB *p = malloc(sizeof(PCB));

    // init process in process table
    p->waiting_time = 0;
    p->execution_time = 0;
    p->remaining_time = (message.process).runningtime;

    // mmry
    p->memsize = pow(2, ceil(log2(pro->memsize)));
    p->index = -1;

    ProcessTable[(message.process).id] = p;

    if (Size_Ready_Queue() == 1 && !is_any_process_running) // i have only one process in ready Q and no processes are running
    {

        processData pro = *(DequeueProcess(ready_queue));

        current_process = &pro;
        current_process_id = current_process->id;
        current_process_start_time = getClk();

        // mmry
        ProcessTable[current_process_id]->index = allocate(pro.memsize);
        memoryFile(0, current_process_id, ProcessTable[current_process_id]->index, ProcessTable[current_process_id]->memsize);

        if (algorithm == RR && current_process->remainingtime >= quantum)
        {
            CreateProcess(quantum);
        }
        else if (algorithm == RR && current_process->remainingtime < quantum)
        {
            CreateProcess(current_process->remainingtime);
        }
        else
        {
            CreateProcess(current_process->remainingtime);
        }

        ProcessTable[current_process_id]->state = RUNNING;
        ProcessTable[current_process_id]->waiting_time = getClk() - current_process->arrivaltime;
        logfile('s', 0, 0);
    }

    if (algorithm == SRTN && is_any_process_running && (message.process).runningtime < get_current_remaining())
    {
        // mmry
        int index = allocate(pro->memsize);
        if (index != -1)
        {
            memoryFile(0, pro->id, index, ProcessTable[pro->id]->memsize);
            ProcessTable[pro->id]->index = index;
            kill(SIGINT, pid);
            SwitchContext(current_process_id);
        }
    }

    up(sem);
}

void SwitchContext(int previous_id) // we need to choose the place which will contain all the updated numbers (ProcessTable or ProcessData)
{

    if (ProcessTable[previous_id]->remaining_time != 0)
    {
        ProcessTable[previous_id]->execution_time += getClk() - current_process_start_time; // check
        ProcessTable[previous_id]->waiting_time = getClk() - current_process->arrivaltime - (current_process->runningtime - current_process->remainingtime);
        ProcessTable[previous_id]->state = WAITING;

        logfile('d', 0, 0);
    }

    if (Size_Ready_Queue() > 0)
    {
        int index = -1;
        bool allocated = false;
        if (algorithm == RR)
        {
            Node *prev, *cur;
            prev = cur = ((Queue *)waiting_queue)->front;
            while (cur && index == -1)
            {
                index = ProcessTable[cur->data->id]->index;
                if (index == -1) // not allocated before
                {
                    index = allocate(cur->data->memsize);
                    if (index != -1)
                        allocated = true;
                }
                if (index != -1)
                {
                    if (prev == cur)
                        ((Queue *)waiting_queue)->front = cur->next;
                    else
                        prev = cur->next;

                    current_process = cur->data;
                    free(cur);
                }
                prev = cur;
                cur = cur->next;
            }

            while (index == -1)
            {
                current_process = DequeueProcess(ready_queue);
                int cur_index = ProcessTable[current_process->id]->index;
                if (cur_index != -1) // alreay allocated
                {
                    index = cur_index;
                    break;
                }

                index = allocate(current_process->memsize);
                if (index != -1)
                    allocated = true;

                if (index == -1)
                    EnqueueProcess(waiting_queue, current_process);
            }
        }
        else if (algorithm == SRTN)
        {
            while (index == -1)
            {
                current_process = DequeueProcess(ready_queue);
                int cur_index = ProcessTable[current_process->id]->index;
                if (cur_index != -1) // alreay allocated
                {
                    index = cur_index;
                    break;
                }

                index = allocate(current_process->memsize);
                if (index != -1)
                    allocated = true;

                if (index == -1)
                    EnqueueProcess(waiting_queue, current_process);
            }

            do
            {
                processData *tmpProcess = DequeueProcess(waiting_queue);
                EnqueueProcess(ready_queue, current_process);

            } while (size_prioQ((PriorityQueue *)waiting_queue) != 0);
        }
        else
        {
            current_process = DequeueProcess(ready_queue);
            index = allocate(current_process->memsize);
            allocated = true;
        }

        current_process_id = current_process->id;
        current_process_start_time = getClk();
        ProcessTable[current_process_id]->state = RUNNING;

        ProcessTable[current_process_id]->index = index;

        if (allocated)
            memoryFile(0, current_process_id, index, ProcessTable[current_process_id]->memsize);

        if (current_process->runningtime == current_process->remainingtime) // first time to start
        {
            ProcessTable[current_process_id]->waiting_time = getClk() - current_process->arrivaltime; /// process started
            logfile('s', 0, 0);
        }
        else
        {
            ProcessTable[current_process_id]->waiting_time = getClk() - current_process->arrivaltime - (current_process->runningtime - current_process->remainingtime); /// process resumed
            logfile('r', 0, 0);
        }

        if (algorithm == RR && ProcessTable[current_process_id]->remaining_time > quantum)
            CreateProcess(quantum);
        else
            CreateProcess(ProcessTable[current_process_id]->remaining_time);
    }
}

void Performance_file()

{

    float sum = 0.0, mean = 0.0, SD = 0.0;
    FILE *perf = fopen("Kernel/performancefile.txt", "w");
    if (perf == NULL)
    {
        perror("unable to open the file");
        exit(0);
    }

    // cpu utlization

    fprintf(perf, "cpu utilization =%.2f\n", ((float)Total_running_time / (getClk() - initial_start_time)) * 100);

    // Calculating Mean
    for (int i = 0; i < NP; i++)
        sum += WTA_arr[i];

    mean = sum / NP;

    fprintf(perf, "Avg WTA = %.2f\n", (float)Total_WTA / NP);
    fprintf(perf, "Avg Waiting = %.2f\n", (float)Total_Waiting / NP);

    // Calculating Standard Deviation
    sum = 0.0;
    for (int i = 0; i < NP; i++)
        sum = sum + (WTA_arr[i] - mean) * (WTA_arr[i] - mean);

    SD = sqrt(sum / NP);
    fprintf(perf, "Std WTA %.2f \n", SD);
    fclose(perf);
}

void logfile(char state, int TA, float WTA)
{
    switch (state)
    {
    case 's':
        fprintf(myfile, "At time %d process %d started  arr  %d  total %d  remain %d  wait %d \n", getClk(), current_process_id, current_process->arrivaltime, current_process->runningtime, ProcessTable[current_process_id]->remaining_time, ProcessTable[current_process_id]->waiting_time); /// started
        break;
    case 'd':
        fprintf(myfile, "At time %d process %d stopped  arr  %d  total %d  remain %d  wait %d \n", getClk(), current_process_id, current_process->arrivaltime, current_process->runningtime, ProcessTable[current_process_id]->remaining_time, ProcessTable[current_process_id]->waiting_time); /// stopped
        break;
    case 'f':
        fprintf(myfile, "At time %d process %d finished  arr  %d  total %d  remain %d  wait %d  TA  %d  WTA  %.2f  \n", getClk(), current_process_id, current_process->arrivaltime, current_process->runningtime, ProcessTable[current_process_id]->remaining_time, ProcessTable[current_process_id]->waiting_time, TA, WTA); // finished
        break;
    case 'r':
        fprintf(myfile, "At time %d process %d Resumed arr  %d  total %d  remain %d  wait %d \n", getClk(), current_process_id, current_process->arrivaltime, current_process->runningtime, ProcessTable[current_process_id]->remaining_time, ProcessTable[current_process_id]->waiting_time); /// resumed
        break;
    }

    fflush(myfile);
}

// mmry
void memoryFile(int state, int id, int start, int size)
{
    if (state == 0)
        fprintf(memFile, "#At time %d allocated %d bytes for process %d from %d to %d \n", getClk(), size, id, start, start + size - 1);
    else
        fprintf(memFile, "#At time %d freed %d bytes from process %d from %d to %d \n", getClk(), size, id, start, start + size - 1);

    fflush(memFile);
}