#ifndef CLION2_COMMON_H
#define CLION2_COMMON_H

#define MAXCLIENTS 1000

typedef struct Shm
{
    int seatLimit;
    int clients;
    int asleep;
    int currShaved;
    int seats[MAXCLIENTS];
} Shm;

const char *memPath = "/Shm";
const char *seats = "/SEATS";
const char *asleep = "/ASLEEP";
const char *wakeup = "/WAKEUP";
const char *sit = "/SIT";
const char *shave = "/SHAVE";
const char *endshave = "/ENDSHAVE";
const char *left = "/LEFT";

#endif //CLION2_COMMON_H
