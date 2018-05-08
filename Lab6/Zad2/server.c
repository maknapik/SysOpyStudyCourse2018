#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
/*************************************************************************/
#include "common.h"
/*************************************************************************/
int serverQueue = 0;
int clients = 0;
int clientsTable[MAXCLIENTS][2];
int wait = 1;
/*************************************************************************/
void deleteQueue()
{
    if(serverQueue > 0)
    {
        for(int i = 0 ; i < clients ; i++)
        {
            mq_close(clientsTable[i][1]);
            kill(clientsTable[i][0], SIGINT);
        }
        if(mq_close(serverQueue) < 0)
        {
            printf("Cannot stop server queue atexit.\n");
        }
        if(mq_unlink(serverPath) < 0)
        {
            printf("Cannot unlink server queue.\n");
        }
    }
}
/*************************************************************************/
void signalInt(int signo)
{
    if(signo == SIGINT) exit(EXIT_SUCCESS);
}
/*************************************************************************/
int main()
{
    if(atexit(deleteQueue) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signalInt);

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MSGSLIMIT;
    posixAttr.mq_msgsize = MAXMSGSZ;

    serverQueue = mq_open(serverPath, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr);
    if(serverQueue == 0)
    {
        printf("Cannot open server queue.\n");
        exit(EXIT_FAILURE);
    }

    struct mq_attr msgQueue;
    Message msg;

    while(1)
    {
        if(wait == 0)
        {
            if(mq_getattr(serverQueue, &msgQueue) != 0)
            {
                printf("Cannot get server queue state.\n");
                exit(EXIT_FAILURE);
            }
            if(msgQueue.mq_curmsgs == 0)
            {
                printf("Server ends queue.\n");
                exit(EXIT_SUCCESS);
            }
        }
        if(mq_receive(serverQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
        {
            printf("Cannot receive message from queue.\n");
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
    char clientPath[50];
    int clientPID = msg->pid;
    sprintf(clientPath, "/%d", clientPID);
    int clientQueue;
    clientQueue = mq_open(clientPath, O_WRONLY);
    if(clientQueue == -1)
    {
        printf("Cannot open client queue.\n");
        exit(EXIT_FAILURE);
    }
    msg->type = START;
    msg->pid - getpid();

    if(clients >= MAXCLIENTS)
    {
        printf("Cannot serve client. Too much active clients.\n");
        sprintf(msg->text, "%d", -1);

        if(mq_send(clientQueue, (char *)msg, MAXMSGSZ, 1) == -1)
        {
            printf("Cannot send message to client.\n");
            exit(EXIT_FAILURE);
        }
        if(mq_close(clientQueue) < 0)
        {
            printf("Cannot close client queue.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        clientsTable[clients][0] = clientPID;
        clientsTable[clients][1] = clientQueue;
        sprintf(msg->text, "%d", clients);
        clients++;
        if(mq_send(clientQueue, (char *)msg, MAXMSGSZ, 1) == -1)
        {
            printf("Cannot send START message to client.\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Server registered a new client.\n");
        }
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
    if(mq_send(clientQueue, (char *)msg, MAXMSGSZ, 2) == -1)
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
        if(mq_send(clientQueue, (char *)msg, MAXMSGSZ, 2) == -1)
        {
            printf("Cannot send CALC message to client.\n");
            exit(EXIT_FAILURE);
        }
    }
    sprintf(msg->text, "%d", result);
    if(mq_send(clientQueue, (char *)msg, MAXMSGSZ, 2) == -1)
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
    if(mq_send(clientQueue, msg, MAXMSGSZ, 0) == -1)
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
