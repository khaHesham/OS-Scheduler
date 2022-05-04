#include "../Defs.h"
#include "../Clock/clk_interface.h"


int remainingtime;

int main(int argc, char * argv[])
{
    initClk();

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
