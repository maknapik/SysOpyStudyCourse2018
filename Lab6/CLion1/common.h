#ifndef CLION1_COMMON_H
#define CLION1_COMMON_H

#define MAXCLIENTS 20
#define MAXMSGSZ 512
#define ID 1001

typedef enum msgType
{
    START = 0, MIRROR = 1, CALC = 2, TIME = 3, END = 4, STOP = 5
} msgType;

typedef struct Message
{
    int type;
    pid_t pid;
    char text[MAXMSGSZ];
} Message;

#endif //CLION1_COMMON_H
