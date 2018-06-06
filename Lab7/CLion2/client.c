#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
/******************************************************/
#include "common.h"
/******************************************************/
Shm *shm;
int flag = 0;
/******************************************************/
sem_t *seatsSem;
sem_t *asleepSem;
sem_t *wakeupSem;
sem_t *sitSem;
sem_t *shaveSem;
sem_t *endshaveSem;
sem_t *leftSem;
/******************************************************/
long getTime()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_nsec / 1000;
}
/******************************************************/
void deleteSem()
{
    if(sem_close(seatsSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(asleepSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(wakeupSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(sitSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(shaveSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(endshaveSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(leftSem) < 0) printf("Error during deleting semaphores.\n");
}
/******************************************************/
void changeFlag(int signo)
{
    if (signo == SIGUSR1) flag = 1;
}
/******************************************************/
void sigExit(int signo)
{
    exit(EXIT_SUCCESS);
}
/******************************************************/
int main(int argc, char *argv[]) {
    if (atexit(deleteSem) < 0) {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigExit);
    signal(SIGINT, sigExit);
    signal(SIGUSR1, changeFlag);

    if (argc < 3) {
        printf("Wrong number of arguments.\n");
    }

    int clients = strtol(argv[1], NULL, 10);
    int shaves = strtol(argv[2], NULL, 10);

    if((seatsSem = sem_open(seats, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((asleepSem = sem_open(asleep, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((wakeupSem = sem_open(wakeup, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((sitSem = sem_open(sit, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((shaveSem = sem_open(shave, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((endshaveSem = sem_open(endshave, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((leftSem = sem_open(left, 0666)) < 0)
    {
        printf("Cannot get semaphore.\n");
        exit(EXIT_FAILURE);
    }

    int sharedMemory = shm_open(memPath, 0666, DEFFILEMODE);
    if(sharedMemory < 0)
    {
        printf("Cannot get shared memory.\n");
        exit(EXIT_FAILURE);
    }

    shm = (Shm *)mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
    if(shm < 0)
    {
        printf("Cannot get pointer to shm.\n");
        exit(EXIT_FAILURE);
    }

    pid_t clientsTable[MAXCLIENTS];
    int k = 0;

    for (int i = 0; i < clients; i++)
    {
        pid_t child = fork();
        if (child == 0)
        {
            for(int j = 0; j < shaves; j++)
            {
                sem_wait(asleepSem);
                if(shm->asleep == 1)
                {
                    shm->currShaved = getpid();

                    printf("Time: %ld\tClient: %d\tWaking up barber.\n", getTime(), getpid());
                    sem_post(wakeupSem);
                    sem_wait(sitSem);
                    printf("Time: %ld\tClient: %d\tSitting on barber's chair.\n", getTime(), getpid());
                    sem_post(shaveSem);
                    sem_wait(endshaveSem);
                    printf("Time: %ld\tClient: %d\tleaving after being shaved.\n", getTime(), getpid());
                    sem_post(leftSem);
                }
                else
                {
                    sem_wait(seatsSem);
                    if(shm->clients >= shm->seatLimit)
                    {
                        printf("Time: %ld\tClient: %d\tleaving because of lack of free seats.\n", getTime(), getpid());
                        sem_post(asleepSem);
                        sem_post(seatsSem);
                    }
                    else
                    {
                        printf("Time: %ld\tClient: %d\twaiting in the queue.\n", getTime(), getpid());
                        shm->seats[shm->clients] =  getpid();
                        shm->clients++;
                        sem_post(asleepSem);
                        sem_post(seatsSem);
                        while (!flag);
                        flag = 0;
                        sem_wait(sitSem);
                        printf("Time: %ld\tClient: %d\tsitting on barber's chair.\n", getTime(), getpid());
                        shm->currShaved = getpid();
                        sem_post(shaveSem);
                        sem_wait(endshaveSem);
                        printf("Time: %ld\tClient: %d\tleaving after being shaved.\n", getTime(), getpid());
                        sem_post(leftSem);
                    }
                }
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            clientsTable[k] = child;
            k++;
        }
    }
    for(int i = 0; i < k; i++)
    {
        waitpid(clientsTable[k], NULL, 0);
    }

    exit(EXIT_SUCCESS);
}