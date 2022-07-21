#ifndef DEFS_H

#define DEFS_H



#include <stdio.h> //if you don't use scanf/printf change this include

#include <sys/types.h>

#include <sys/stat.h>

#include <sys/file.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <sys/sem.h>

#include <sys/msg.h>

#include <sys/wait.h>

#include <stdlib.h>

#include <unistd.h>

#include <signal.h>



typedef short bool;

#define true 1

#define false 0



#define SHKEY 300

#define MSGQKEY 100

#define SEMKEY 20

typedef enum STATE

{

    RUNNING,

    WAITING,

}STATE;



typedef enum ALGORITHM

{

    RR,

    SRTN,

    HPF,

}ALGORITHM;





typedef struct PCB 
{

    enum STATE state;
    int remaining_time;
    int execution_time;
    int waiting_time;

    //mmry
    int memsize;
    int index;

} PCB;


typedef struct processData

{

    int arrivaltime;

    int priority;

    int runningtime;

    int remainingtime;//Initially equals runningtime!!    ////updated

    int id;

    //mmry
    int memsize;

}processData;



typedef struct msgbuff{
    
    long mtype;
    processData process;
    //char mtext[50];

}msgbuff;




#endif
