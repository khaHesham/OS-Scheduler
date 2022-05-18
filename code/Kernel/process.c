#include "../Defs.h"

#include "../Clock/clk_interface.h"



void handler(int signum);



int remainingtime;



int main(int argc, char * argv[])

{

    initClk();

    signal(SIGINT,handler);



    if(argc != 2)

        return(1);



    remainingtime = atoi(argv[1]);



    int finish_time = getClk() + remainingtime;



    while(remainingtime > 0)

    {

        remainingtime = finish_time - getClk();

    }



    kill(getppid(), SIGUSR1);

    

    destroyClk(false);

    

    return 0;

}



void handler(int signum)

{

 destroyClk(false);

 signal(SIGINT,handler);

 exit(0);

}