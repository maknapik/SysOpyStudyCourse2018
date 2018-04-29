#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <sys/msg.h>
#include <bits/signum.h>
#include <signal.h>
#include <unistd.h>
/*********************************************************************************/
#include "common.h"
/*********************************************************************************/
#define MAXLINE 128
/*********************************************************************************/
int clientQueue = 0;
int serverQueue = 0;
/*********************************************************************************/
void deleteQueue()
{
    if(clientQueue > 0)
    {
        if(msgctl(clientQueue, IPC_RMID, NULL) < 0) printf("Cannot remove client queue atexit.\n");
    }
    if(serverQueue > 0)
    {
        Message msg;
        msg.type = STOP;
        msg.pid = getpid();

        if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0) printf("Cannot send stop message to server.\n");
        else printf("Client sent stop message to server.\n");
    }
}
/*********************************************************************************/
void fileInput(char *filename)
{
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        printf("Cannot open the file.\n");
        exit(EXIT_FAILURE);
    }

    Message msg;
    char line[MAXLINE];

    while(fgets(line, MAXLINE, file))
    {
        char *task = line;
        if(strcmp(task, "\n") == 0) continue;

        char *operation = strtok_r(NULL, " \n\t", &task);
        if(strcmp(operation, "MIRROR") == 0)
        {
            msg.type = MIRROR;
            if(line == NULL)
            {
                printf("There is a blank line after MIRROR operation.\n");
                exit(EXIT_FAILURE);
            }
            sprintf(msg.text, "%s", task);
            msg.pid = getpid();
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send MIRROR message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive MIRROR message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(operation, "CALC") == 0)
        {
            msg.type = CALC;
            if(line == NULL)
            {
                printf("There is a blank line after CALC operation.\n");
                exit(EXIT_FAILURE);
            }
            sprintf(msg.text, "%s", task);
            msg.pid = getpid();
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send CALC message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive CALC message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);

        }
        else if(strcmp(operation, "TIME") == 0)
        {
            msg.type = TIME;
            msg.pid = getpid();
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send TIME message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive TIME message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(operation, "END") == 0)
        {
            msg.type = END;
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send END message to server.\n");
                exit(EXIT_FAILURE);
            }
            fclose(file);
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized operation.\n");
            continue;
        }
    }
}
/*********************************************************************************/
void cmdInput()
{
    Message msg;
    char line[MAXLINE];

    while(1)
    {
        msg.pid = getpid();
        printf("Enter operation: ");
        if(fgets(line, MAXLINE, stdin) == NULL)
        {
            printf("Error during reading operation.\n");
            continue;
        }

        int i = strlen(line);
        if(line[i - 1] == '\n') line[i - 1] = 0;

        if(strcmp(line, "MIRROR") == 0)
        {
            msg.type = MIRROR;
            printf("Enter string to make MIRROR operation: ");
            if(fgets(msg.text, MAXMSGSZ, stdin) == NULL)
            {
                printf("Too long string.\n");
                continue;
            }
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send MIRROR message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive MIRROR message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(line, "CALC") == 0)
        {
            msg.type = CALC;
            printf("Enter CALC expression: ");
            if(fgets(msg.text, MAXMSGSZ, stdin) == NULL)
            {
                printf("Too long expression.\n");
                continue;
            }
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send CALC message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive CALC message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(line, "TIME") == 0)
        {
            msg.type = TIME;
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send TIME message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
            {
                printf("Cannot receive TIME message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(line, "END") == 0)
        {
            msg.type = END;
            if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
            {
                printf("Cannot send END message to server.\n");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized operation.\n");
        }
    }
}
/*********************************************************************************/
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    if(atexit(deleteQueue) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    char *path = getenv("HOME");
    if(path == NULL)
    {
        printf("Cannot get HOME envvar.\n");
        exit(EXIT_FAILURE);
    }

    key_t serverKey = ftok(path, ID);
    serverQueue = msgget(serverKey, 0);
    if(serverQueue < 0)
    {
        printf("Cannot get server's queue.\n");
        exit(EXIT_FAILURE);
    }

    key_t clientKey = ftok(path, getpid());
    clientQueue = msgget(clientKey, IPC_CREAT | 0666);
    if(clientQueue < 0)
    {
        printf("Cannot make client's queue\n");
        exit(EXIT_FAILURE);
    }

    Message msg;
    msg.type = START;
    msg.pid = getpid();
    sprintf(msg.text, "%d", clientKey);
    if(msgsnd(serverQueue, &msg, MAXMSGSZ, 0) < 0)
    {
        printf("Cannot send start message to server.\n");
        exit(EXIT_FAILURE);
    }

    if(msgrcv(clientQueue, &msg, MAXMSGSZ, 0, 0) < 0)
    {
        printf("Cannot receive start permission message to server.\n");
        exit(EXIT_FAILURE);
    }

    int clientID = atoi(msg.text);
    if(clientID == -1)
    {
        printf("There is no place for client.\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "f") == 0)
    {
        fileInput(argv[2]);
    }
    else if(strcmp(argv[1], "c") == 0)
    {
        cmdInput();
    }
    else
    {
        printf("Wrong argument.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}