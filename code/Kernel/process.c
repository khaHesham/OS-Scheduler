#include "../Defs.h"

#include "../Clock/clk_interface.h"

void handler(int signum);

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

int remainingtime;
union Semun semun;
int sem;

int main(int argc, char *argv[])

{

    initClk();
    signal(SIGINT, handler);

    sem = semget(SEMKEY, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    if (argc != 2)
        return (1);

    remainingtime = atoi(argv[1]);
    int finish_time = getClk() + remainingtime;

    while (remainingtime > 0)
    {
        remainingtime = finish_time - getClk();
    }

    down(sem);

    kill( SIGUSR1,getppid());
    destroyClk(false);

    return 0;
}

void handler(int signum)

{

    destroyClk(false);
    signal(SIGINT, handler);
    exit(0);
}
