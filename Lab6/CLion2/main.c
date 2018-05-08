#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
/*************************************************************************/
#include "common.h"
/*************************************************************************/
#define MAXLINE 128
/*************************************************************************/
int clientID = 0;
int clientQueue = 0;
int serverQueue = 0;
char path[50];
/*************************************************************************/
void deleteQueue()
{
    if(serverQueue > 0 || clientQueue > 0)
    {
        Message msg;
        msg.type = STOP;
        msg.pid = getpid();

        if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
        {
            printf("Cannot send stop message to server.\n");
            exit(EXIT_FAILURE);
        }
        if(mq_close(serverQueue) < 0)
        {
            printf("Cannot stop server queue atexit.\n");
        }
        if(mq_close(clientQueue) < 0)
        {
            printf("Cannot stop client queue atexit.\n");
        }
        if(mq_unlink(path) < 0)
        {
            printf("Cannot delete client queue atexit.\n");
        }
    }
}
/*************************************************************************/
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
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send MIRROR message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
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
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send CALC message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
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
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send TIME message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
            {
                printf("Cannot receive TIME message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(operation, "END") == 0)
        {
            msg.type = END;
            msg.pid = getpid();
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
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

    fclose(file);
}
/*************************************************************************/
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
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send MIRROR message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
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
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send CALC message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
            {
                printf("Cannot receive CALC message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(line, "TIME") == 0)
        {
            msg.type = TIME;
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
            {
                printf("Cannot send TIME message to server.\n");
                exit(EXIT_FAILURE);
            }
            if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) < 0)
            {
                printf("Cannot receive TIME message from server.\n");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", msg.text);
        }
        else if(strcmp(line, "END") == 0)
        {
            msg.type = END;
            if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 2) < 0)
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
/*************************************************************************/
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    };

    if(atexit(deleteQueue) < 0)
    {
        printf("Cannot register atexit function.\n");
        exit(EXIT_FAILURE);
    }

    sprintf(path, "/%d", getpid());

    serverQueue = mq_open(serverPath, O_WRONLY);
    if(serverQueue == 0)
    {
        printf("Cannot open server queue.\n");
        exit(EXIT_FAILURE);
    }

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MSGSLIMIT;
    posixAttr.mq_msgsize = MAXMSGSZ;

    clientQueue = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr);
    if(serverQueue == 0)
    {
        printf("Cannot open client queue.\n");
        exit(EXIT_FAILURE);
    }

    Message msg;
    msg.type = START;
    msg.pid = getpid();

    if(mq_send(serverQueue, (char *)&msg, MAXMSGSZ, 1) == -1)
    {
        printf("Cannot login.\n");
        exit(EXIT_FAILURE);
    }
    if(mq_receive(clientQueue, (char *)&msg, MAXMSGSZ, NULL) == -1)
    {
        printf("Cannot receive START message.\n");
        exit(EXIT_FAILURE);
    }
    int clientID = strtol(msg.text, NULL, 10);
    if(clientID < 0)
    {
        printf("Server reached max clients.\n");
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
}