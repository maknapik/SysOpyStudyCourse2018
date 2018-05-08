#ifndef CLION2_COMMON_H
#define CLION2_COMMON_H

#define MAXCLIENTS 20
#define MAXMSGSZ 512
#define MSGSLIMIT 20

const char serverPath[] = "/server";

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

#endif //CLION2_COMMON_H
