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
/******************************************************/
#include "common.h"
/******************************************************/
int sharedMemory = 0;
/******************************************************/
sem_t *seatsSem;
sem_t *asleepSem;
sem_t *wakeupSem;
sem_t *sitSem;
sem_t *shaveSem;
sem_t *endshaveSem;
sem_t *leftSem;
/******************************************************/
Shm *shm;
/******************************************************/
long getTime()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_nsec / 1000;
}
/******************************************************/
void sigExit(int signo)
{
    exit(EXIT_SUCCESS);
}
/******************************************************/
void deleteShmSem()
{
    if(shm_unlink(memPath) < 0) printf("Error during deleting shm.\n");
    if(sem_close(seatsSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(asleepSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(wakeupSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(sitSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(shaveSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(endshaveSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_close(leftSem) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(seats) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(asleep) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(wakeup) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(sit) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(wakeup) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(endshave) < 0) printf("Error during deleting semaphores.\n");
    if(sem_unlink(left) < 0) printf("Error during deleting semaphores.\n");

}
/******************************************************/
int main(int argc, char *argv[])
{
    if(atexit(deleteShmSem) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    if(argc < 2)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigExit);
    signal(SIGINT, sigExit);

    if((seatsSem = sem_open(seats, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((asleepSem = sem_open(asleep, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((wakeupSem = sem_open(wakeup, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((sitSem = sem_open(sit, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((shaveSem = sem_open(shave, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((endshaveSem = sem_open(endshave, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }
    if((leftSem = sem_open(left, O_CREAT | 0666, DEFFILEMODE, 1)) < 0)
    {
        printf("Cannot make semaphore.\n");
        exit(EXIT_FAILURE);
    }

    sharedMemory = shm_open(memPath, O_CREAT | O_RDWR | O_TRUNC, DEFFILEMODE);
    if(sharedMemory < 0)
    {
        printf("Cannot create shm.\n");
        exit(EXIT_FAILURE);
    }
    ftruncate(sharedMemory, sizeof(Shm));

    shm = (Shm *)mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
    if (shm < 0)
    {
        printf("Cannot get pointer to shm.\n");
        exit(EXIT_FAILURE);
    }
    shm->seatLimit = strtol(argv[1], NULL, 10);
    shm->clients = 0;
    for(int i = 0; i < strtol(argv[1], NULL, 10); i++)
    {
        shm->seats[i] = 0;
    }

    while (1)
    {
        sem_wait(asleepSem);
        sem_wait(seatsSem);
        printf("%d clients left.\n", shm->clients);
        if (shm->clients == 0)
        {
            sem_post(seatsSem);
            printf("Time: %ld\tGOLIBRODA: Falling asleep.\n", getTime());
            shm->asleep = 1;
            sem_post(asleepSem);
            sem_wait(wakeupSem);
            shm->asleep = 0;
            sem_post(asleepSem);
            printf("Time: %ld\tGOLIBRODA: Waking up.\n", getTime());
            sem_post(sitSem);
            sem_wait(shaveSem);
            printf("Time: %ld\tGOLIBRODA: Shaving client with PID: %d\n", getTime(), shm->currShaved);
            printf("Time: %ld\tGOLIBRODA: Ended shaving client with PID: %d\n", getTime(), shm->currShaved);
            sem_post(endshaveSem);
            sem_wait(leftSem);
        } 
        else 
        {
            printf("Time: %ld\tGOLIBRODA: Inviting client with PID %d to sit.\n", getTime(), shm->seats[0]);
            kill(shm->seats[0], SIGUSR1);
            sem_post(asleepSem);
            sem_post(sitSem);
            for (int i = 0; i < shm->clients; i++) shm->seats[i] = shm->seats[i+1];
            shm->clients--;
            sem_post(seatsSem);
            sem_wait(shaveSem);
            printf("Time: %ld\tGOLIBRODA: Shaving client with PID: %d\n", getTime(), shm->currShaved);
            printf("Time: %ld\tGOLIBRODA: Ended shaving client with PID: %d\n", getTime(), shm->currShaved);
            sem_post(endshaveSem);
            sem_wait(leftSem);
        }
    }
    exit(EXIT_SUCCESS);
}
/******************************************************/
