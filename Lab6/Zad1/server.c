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
            case STOP:
                stopTask(&msg);
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
        clientsTable[clients][0] = clientPID;
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

    char tmp;
    size_t length = strlen(message) - 1;
    size_t j = length;
    for(int i = 0 ; i < length / 2 ; i++, j--)
    {
        tmp = message[i];
        message[i] = message[j];
        message[j] = tmp;
    }
    sprintf(msg->text, "%s", message);
    if(msgsnd(clientQueue, msg, MAXMSGSZ, 0) == -1)
    {
        printf("Cannot send MIRROR message to client.\n");
        exit(EXIT_FAILURE);
    }
}
/*********************************************************************************/
void calcTask(struct Message *msg)
{
    int clientQueue = getClientQueue(msg);
    if(clientQueue == -1)
    {
        printf("Cannot find client's ID.\n");
        exit(EXIT_FAILURE);
    }

    char *message = msg->text;
    char *line = message;
    char *operation;
    char *firstArg;
    char *secondArg;
    int result = 0;

    if(strcmp(message, "\n") == 0)
    {
        printf("There is a blank line in CALC message.\n");
        exit(EXIT_FAILURE);
    }
    printf("text: %s\n", msg->text);
    operation = strtok_r(NULL, " \n\t", &line);
    firstArg = strtok_r(NULL, " \n\t", &line);
    secondArg = strtok_r(NULL, " \n\t", &line);
    if(strcmp(operation, "ADD") == 0) result = (int)(strtol(firstArg, NULL, 10) + strtol(secondArg, NULL, 10));
    else if(strcmp(operation, "SUB") == 0) result = (int)(strtol(firstArg, NULL, 10) - strtol(secondArg, NULL, 10));
    else if(strcmp(operation, "MUL") == 0) result = (int)(strtol(firstArg, NULL, 10) * strtol(secondArg, NULL, 10));
    else if(strcmp(operation, "DIV") == 0 && strtol(secondArg, NULL, 10) != 0) result = (int)(strtol(firstArg, NULL, 10) / strtol(secondArg, NULL, 10));
    else
    {
        sprintf(msg->text, "%s", "NaN");
        printf("Cannot resolve expression.\n");
        if(msgsnd(clientQueue, msg, MAXMSGSZ, 0) == -1)
        {
            printf("Cannot send CALC message to client.\n");
            exit(EXIT_FAILURE);
        }
    }
    sprintf(msg->text, "%d", result);
    if(msgsnd(clientQueue, msg, MAXMSGSZ, 0) == -1)
    {
        printf("Cannot send CALC message to client.\n");
        exit(EXIT_FAILURE);
    }
}
/*********************************************************************************/
void timeTask(struct Message *msg)
{
    int clientQueue = getClientQueue(msg);
    if(clientQueue == -1)
    {
        printf("Cannot find client's ID.\n");
        exit(EXIT_FAILURE);
    }

    char timeText[20];
    time_t actual = time(NULL);
    strftime(timeText, 20, "%Y-%m-%d %H:%M:%S", localtime(&actual));
    sprintf(msg->text, "%s", timeText);
    if(msgsnd(clientQueue, msg, MAXMSGSZ, 0) == -1)
    {
        printf("Cannot send TIME message to client.\n");
        exit(EXIT_FAILURE);
    }
}
/*********************************************************************************/
void stopTask(struct Message *msg)
{
    int i = 0;
    for(; i < MAXCLIENTS ; i++)
    {
        if(clientsTable[i][0] == msg->pid) break;
    }

    for(; i < clients ; i++)
    {
        clientsTable[i][0] = clientsTable[i + 1][0];
        clientsTable[i][1] = clientsTable[i + 1][1];
    }
    clients--;
    printf("Server removed client.\n");
}
/*********************************************************************************/
int getClientQueue(struct Message *msg)
{
    int clientQueue = -1;

    for(int i = 0 ; i < MAXCLIENTS ; i++)
    {
        if(clientsTable[i][0] == msg->pid) clientQueue = clientsTable[i][1];
    }

    msg->type = msg->pid;
    msg->pid = getpid();
    return clientQueue;
}