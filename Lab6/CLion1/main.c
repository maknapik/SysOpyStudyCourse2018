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

    if(strcmp(argv[1], 'f') == 0)
    {
        fileInput(argv[2]);
    }
    if(strcmp(argv[2], 'c') == 0)
    {
        cmdInput();
    }
    else
    {
        printf("Wrong argument.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}