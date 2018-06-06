#ifndef CLION1_COMMON_H
#define CLION1_COMMON_H

#define ID 1001
#define MAXCLIENTS 100
#define SEMPATH "./"
#define SHMPATH "/"

#define ASLEEP 1
#define WAKEUP 2
#define SHAVE 3
#define ENDSHAVE 4
#define LEFT 5
#define SIT 6
#define SEATS 7

typedef struct Shm
{
    int seatLimit;
    int clients;
    int asleep;
    int currShaved;
    int seats[MAXCLIENTS];
} Shm;

#endif //CLION1_COMMON_H
