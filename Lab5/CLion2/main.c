#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <time.h>
#include <sys/wait.h>
/************************************************************/
pid_t mainPid;
pid_t childPids[1000];
/************************************************************/
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0 ; i < atoi(argv[2]) ; i++)
    {
        childPids[i] = fork();
        if (childPids[i] == 0)
        {
            execlp("./slave", "slave", argv[1]);
        }
    }

    mainPid = fork();
    if (mainPid == 0)
    {
        execlp("./master", "master", argv[1], argv[3]);
    }
    else
    {
        for (int i = 0 ; i < atoi(argv[2]) ; i++)
        {
            waitpid(childPids[i], NULL, WUNTRACED);
        }
    }

    kill(mainPid, SIGINT);
    exit(EXIT_SUCCESS);
}
