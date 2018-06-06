#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>
/*************************************************************/
#include "common.h"
/*************************************************************/
int memID = 0;
int semID = 0;
Shm *shm;
/*************************************************************/
long getTime()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_nsec / 1000;
}
/*************************************************************/
void sigExit(int signo)
{
    exit(EXIT_SUCCESS);
}
/*************************************************************/
void deleteShmSem()
{
    if(shmctl(memID, IPC_RMID, NULL) < 0)
    {
        printf("Something went wrong during deleting shm.\n");
    }
    if(semctl(semID, 0, IPC_RMID, NULL) < 0)
    {
        printf("Something went wrong during deleting semIDs.\n");
    }
}
/*************************************************************/
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    if(atexit(deleteShmSem) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigExit);
    signal(SIGINT, sigExit);

    key_t semKey = ftok(SEMPATH, ID);

    semID = semget(semKey, 7, IPC_CREAT | 0666);
    if(semID < 0)
    {
        printf("Cannot create semID.\n");
        exit(EXIT_FAILURE);
    }

    key_t memKey = ftok(SHMPATH, ID);
    memID = shmget(memKey, sizeof(Shm), IPC_CREAT | 0666);
    if(memID < 0)
    {
        printf("Cannot create memID.\n");
        exit(EXIT_FAILURE);
    }

    shm = (Shm*)shmat(memID, NULL, 0);
    if(shm < 0)
    {
        printf("Cannot get pointer to Shm.\n");
        exit(EXIT_FAILURE);
    }

    shm->seatLimit = strtol(argv[1], NULL, 10);
    shm->clients = 0;

    for(int i = 0 ; i < 2 ; i++)
    {
        semctl(semID, i, SETVAL, 1);
    }

    for(int i = 2 ; i < 7 ; i++)
    {
        semctl(semID, i, SETVAL, 0);
    }

    for(int i = 0 ; i < strtol(argv[1], NULL, 10) ; i++)
    {
        shm->seats[i] = 0;
    }

    struct sembuf waitSem;
    waitSem.sem_op = -1;
    waitSem.sem_flg = 0;

    struct sembuf unblockSem;
    unblockSem.sem_op = 1;
    unblockSem.sem_flg = 0;

    while(1)
    {
        waitSem.sem_num = ASLEEP;
        semop(semID, &waitSem, 1);
        waitSem.sem_num = SEATS;
        semop(semID, &waitSem, 1); // czekanie na mozliwosc sprawdzenia tablicy klientow i ich ilosci
        printf("%d clients left.\n", shm->clients);
        if (shm->clients == 0)
        {
            unblockSem.sem_num = SEATS;
            semop(semID, &unblockSem, 1); // odblokowuje modyfikacje tablicy klientow
            printf("Time: %ld\tGOLIBRODA: Falling asleep.\n", getTime());
            shm->asleep = 1;
            unblockSem.sem_num = ASLEEP;
            semop(semID, &unblockSem, 1); //odblokowanie sprawdzania czy spi
            waitSem.sem_num = WAKEUP;
            semop(semID, &waitSem, 1); // czekanie az ktos go obudzi
            shm->asleep = 0;
            unblockSem.sem_num = ASLEEP;
            semop(semID, &unblockSem, 1); // klient zablokowal flage po dowiedzeniu sie ze spie wiec musze ja zmienic
            printf("Time: %ld\tGOLIBRODA: Waking up.\n", getTime());
            unblockSem.sem_num = SIT;
            semop(semID, &unblockSem, 1); // pozwalam usiasc
            waitSem.sem_num = SHAVE;
            semop(semID, &waitSem, 1); // czekam az usiadzie
            printf("Time: %ld\tGOLIBRODA: Shaving client with PID: %d\n", getTime(), shm->currShaved);
            printf("Time: %ld\tGOLIBRODA: Ended shaving client with PID: %d\n", getTime(), shm->currShaved);
            unblockSem.sem_num = ENDSHAVE;
            semop(semID, &unblockSem, 1); // pozwalam odejsc
            waitSem.sem_num = LEFT;
            semop(semID, &waitSem, 1); // czekam az klient wyjdzie zeby zajac sie nastepnymi
        } 
        else
        { // sa klienci
            printf("Time: %ld\tGOLIBRODA: Inviting client with PID %d to sit.\n", getTime(), shm->seats[0]);
            kill(shm->seats[0], SIGUSR1);
            unblockSem.sem_num = ASLEEP;
            semop(semID, &unblockSem, 1); // odblokowanie sprawdzania czy spi bo i tak jest zajety
            unblockSem.sem_num = SIT;
            semop(semID, &unblockSem, 1); // pozwalam usiasc
            for (int i = 0; i <shm->clients; ++i) shm->seats[i] = shm->seats[i+1]; // przesuniecie tablicy
            shm->clients--; // usuniecie klienta
            unblockSem.sem_num = SEATS;
            semop(semID, &unblockSem, 1); // mozna sprawdzac tablice klientow
            waitSem.sem_num = SHAVE;
            semop(semID, &waitSem, 1); // czekam az usiadzie
            printf("Time: %ld\tGOLIBRODA: Shaving client with PID: %d\n", getTime(), shm->currShaved);
            printf("Time: %ld\tGOLIBRODA: Ended shaving client with PID: %d\n", getTime(), shm->currShaved);
            unblockSem.sem_num = ENDSHAVE;
            semop(semID, &unblockSem, 1); // skonczono strzyzenie, klient moze odejsc
            waitSem.sem_num = LEFT;
            semop(semID, &waitSem, 1); // czekam az klient wyjdzie zeby zajac sie nastepnymi
        }
    }
    exit(EXIT_SUCCESS);
}
