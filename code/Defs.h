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

typedef enum STATE
{
    RUNNING,
    WAITING,
}STATE;

typedef enum ALGORITHM
{
    SRTN,
    HPF,
    RR,
}ALGORITHM;


typedef struct PCB 
{
    enum STATE state;
    int remaining_time;
    int execution_time;
    int waiting_time;

} PCB;

typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int remainingtime;//Initially equals runningtime!!  khaled: I dont think so conflictions will occour we may initialize it with (-1)
    int id;

}processData;


typedef struct messagebuff
{
    long msgtype;
    struct processData process_data;

}messagebuff;

#endif