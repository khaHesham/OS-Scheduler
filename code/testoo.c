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



void handler(int s);
int main()
{
signal(SIGINT,handler);

    while(1)
    {
        pause();
    }
    return 0;
}

void handler(int s)
{

    printf("hellooooooo clownss\n");
    signal(SIGINT,handler);
}