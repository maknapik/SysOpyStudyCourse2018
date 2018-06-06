#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>
/*************************************************************/
#include "common.h"
/*************************************************************/
Shm *shm;
int flag = 0;
/*************************************************************/
long getTime()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_nsec / 1000;
}
/*************************************************************/
void changeFlag(int signo)
{
    if(signo == SIGUSR1) flag = 1;
}
/*************************************************************/
void sigExit(int signo)
{
    exit(EXIT_SUCCESS);
}
/*************************************************************/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigExit);
    signal(SIGINT, sigExit);
    signal(SIGUSR1, changeFlag);

    int clients = strtol(argv[1], NULL, 10);
    int shaves = strtol(argv[2], NULL, 10);

    key_t semKey = ftok(SEMPATH, ID);
    int semID = semget(semKey, 0, 0666);
    if(semID < 0)
    {
        printf("Cannot create semID.\n");
        exit(EXIT_FAILURE);
    }

    key_t memKey = ftok(SHMPATH, ID);
    int memID = shmget(memKey, sizeof(Shm), 0666);
    if(memID < 0)
    {
        printf("Cannot get shared memID.\n");
        exit(EXIT_FAILURE);
    }

    shm = (Shm *)shmat(memID, NULL, 0);
    if(shm < 0)
    {
        printf("Cannot get pointer to Shm.\n");
        exit(EXIT_FAILURE);
    }

    pid_t clientsTable[MAXCLIENTS];
    int k = 0;

    struct sembuf waitSem;
    waitSem.sem_op = -1;
    waitSem.sem_flg = 0;

    struct sembuf unblockSem;
    unblockSem.sem_op = 1;
    unblockSem.sem_flg = 0;

    for(int i = 0 ; i < clients ; i++)
    {
        pid_t client = fork();
        if(client == 0)
        {
            for(int j = 0 ; j < shaves ; j++)
            {
                waitSem.sem_num = ASLEEP;
                semop(semID, &waitSem, 1);
                if(shm->asleep == 1)
                {
                    shm->currShaved = getpid();

                    printf("Time: %ld\tClient: %d\twaking up barber.\n", getTime(), getpid());
                    unblockSem.sem_num = WAKEUP;
                    semop(semID, &unblockSem, 1);
                    waitSem.sem_num = SIT;
                    semop(semID, &waitSem, 1);
                    printf("Time: %ld\tClient: %d\tsitting on barber's chair.\n", getTime(), getpid());
                    unblockSem.sem_num = SHAVE;
                    semop(semID, &unblockSem, 1);
                    waitSem.sem_num = ENDSHAVE;
                    semop(semID, &waitSem, 1);
                    printf("Time: %ld\tClient: %d\tis shaved.\n", getTime(), getpid());
                    unblockSem.sem_num = LEFT;
                    semop(semID, &unblockSem, 1);
                }
                else
                {
                    waitSem.sem_num = SEATS;
                    semop(semID, &waitSem, 1);

                    if(shm->clients >= shm->seatLimit)
                    {
                        printf("Time: %ld\tClient: %d\tleaving because of lack of free seats.\n", getTime(), getpid());
                        unblockSem.sem_num = ASLEEP;
                        semop(semID, &unblockSem, 1);
                        unblockSem.sem_num = SEATS;
                        semop(semID, &unblockSem, 1);
                    }
                    else
                    {
                        printf("Time: %ld\tClient: %d\twaiting in the queue.\n", getTime(), getpid());
                        shm->seats[shm->clients++] = getpid();
                        unblockSem.sem_num = ASLEEP;
                        semop(semID, &unblockSem, 1);
                        unblockSem.sem_num = SEATS;
                        semop(semID, &unblockSem, 1);
                        while(!flag);
                        flag = 0;
                        waitSem.sem_num = SIT;
                        semop(semID, &waitSem, 1);
                        printf("Time: %ld\tClient: %d\tsitting on barber's chair.\n", getTime(), getpid());
                        shm->currShaved = getpid();
                        unblockSem.sem_num = SHAVE;
                        semop(semID, &unblockSem, 1);
                        waitSem.sem_num = ENDSHAVE;
                        semop(semID, &waitSem, 1);
                        printf("Time: %ld\tClient: %d\tleaving after being shaved.\n", getTime(), getpid());
                        unblockSem.sem_num = LEFT;
                        semop(semID, &unblockSem, 1);
                    }
                }
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            clientsTable[k] = client;
            k++;
        }
    }
    for(int i = 0 ; i < k ; i++)
    {
        waitpid(clientsTable[i], NULL, 0);
    }
    exit(EXIT_SUCCESS);
}