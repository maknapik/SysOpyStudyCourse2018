#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
/*********************************************************************************/
#include "common.h"
/*********************************************************************************/
int serverQueue = 0;
int clients = 0;
int clientsTable[MAXCLIENTS][2];
int wait = 1;
/*********************************************************************************/
void deleteQueue()
{
    if(serverQueue > 0)
    {
        if(msgctl(serverQueue, IPC_RMID, NULL) < 0)
        {
            printf("Cannot delete server Queue atexit.\n");
        }

        for(int i = 0 ; i < clients ; i++)
        {
            kill(clientsTable[i][0], SIGINT);
        }
        printf("Server ends work.\n");
    }
}
/*********************************************************************************/
void signalInt(int signo)
{
    if(signo == SIGINT) exit(EXIT_SUCCESS);
}
/*********************************************************************************/
int main()
{
    if(atexit(deleteQueue) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signalInt);

    char *path = getenv("HOME");
    if(path == NULL)
    {
        printf("Cannot get HOME getenv.\n");
        exit(EXIT_FAILURE);
    }

    key_t serverKey = ftok(path, ID);
    serverQueue = msgget(serverKey, IPC_CREAT | 0666);
    if(serverQueue < 0)
    {
        printf("Cannot make server queue.\n");
        exit(EXIT_FAILURE);
    }

    struct msqid_ds msgQueue;
    Message msg;

    while(1)
    {
        if(wait == 0)
        {
            if(msgctl(serverQueue, IPC_STAT, &msgQueue) != 0)
            {
                printf("Cannot get server queue state.\n");
                exit(EXIT_FAILURE);
            }
            if(msgQueue.msg_qnum == 0)
            {
                printf("Server ends server queue.\n");
                break;
            }
        }
        if(msgrcv(serverQueue, &msg, MAXMSGSZ, 0, 0) < 0)
        {
            printf("Cannot receive messsage from queue.\n");
            exit(EXIT_FAILURE);
        }
        switch(msg.type)
        {
            case START:
                startTask(&msg);
                break;
            case MIRROR:
                mirrorTask(&msg);
                break;
            case CALC:
                calcTask(&msg);
                break;
            case TIME:
                timeTask(&msg);
                break;
            case END:
                wait = 0;
                printf("Server got END message.\n");
                break;
            default:
                printf("Unrecognized task type.\n");
                exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
/*********************************************************************************/
void startTask(struct Message *msg)
{
    key_t clientKey = (key_t)strtol(msg->text, NULL, 10);
    int clientQueue = msgget(clientKey, 0);
    int clientPID = msg->pid;
    msg->pid = getpid();
    msg->type = START;

    if(clients >= MAXCLIENTS)
    {
        printf("Cannot serve client. Too much active clients.\n");
        sprintf(msg->text, "%d", -1);
    }
    else
    {
        clientsTable[clients][0] == clientPID;
        clientsTable[clients][1] = clientQueue;
        sprintf(msg->text, "%d", clients);
        clients++;
    }
    if(msgsnd(clientQueue, msg, MAXMSGSZ, 0) == -1)
    {
        printf("Cannot send START message to client.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server registered a new client.\n");
    }
}
/*********************************************************************************/
void mirrorTask(struct Message *msg)
{
    int clientQueue = getClientQueue(msg);
    if(clientQueue == -1)
    {
        printf("Cannot find client's ID.\n");
        exit(EXIT_FAILURE);
    }

    char *message = msg->text;
    if(strlen(message) == 0)
    {
        printf("Empty string in mirror task.\n");
        exit(EXIT_FAILURE);
    }
}
/*********************************************************************************/
/*********************************************************************************/