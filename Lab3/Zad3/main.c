#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/time.h>

void makelimits(char *time, char *memory)
{
    int timelimit = strtol(time, NULL, 10);
    struct rlimit cpulimit;

    cpulimit.rlim_max = (rlim_t)timelimit;
    cpulimit.rlim_cur = (rlim_t)timelimit;

    if(setrlimit(RLIMIT_CPU, &cpulimit) != 0)
    {
        printf("Error while setting CPU limit\n");
        return 0;
    }

    int memorylimit = strtol(memory, NULL, 10);
    struct rlimit rmemorylimit;

    rmemorylimit.rlim_max = (rlim_t)memorylimit * 1024 * 1024;
    rmemorylimit.rlim_cur = (rlim_t)memorylimit * 1024 * 1024;

    if(setrlimit(RLIMIT_DATA, &rmemorylimit) != 0)
    {
        printf("Error while setting memory limit\n");
        return 0;
    }
}
/****************************************************/
int main(int argc, const char* argv[])
{
    if(argc != 4)
    {
        printf("Wrong number of arguments! Give only name of a file to execute commands.\n");
        return 1;
    }

    const int maxlinelength = 100;
    const int maxargsnumber = 10;

    FILE *file = fopen(argv[1], "r");
    if(!file)
    {
        printf("Error while opening the file.\n");
        return 1;
    }

    char *line = malloc(maxlinelength * sizeof(char));
    size_t length;
    char *arguments[maxargsnumber];

    struct rusage prev_usage;
    getrusage(RUSAGE_CHILDREN, &prev_usage);

    while(getline(&line, &length, file) != -1)
    {
        char *arguments[maxargsnumber];
        int args = 0;

        line[strlen(line) - 1] = '\0';
        if(strlen(line) == 1) continue;

        char *tline = strtok(line, " ");
        while(tline)
        {
            arguments[args++] = tline;
            tline = strtok(NULL, " ");
        }
        free(tline);
        arguments[args] = NULL;

        if(arguments[0] == NULL) continue;

        pid_t proc = fork();
        if(proc < 0)
        {
            printf("Error while creating new process.\n");
            exit(EXIT_FAILURE);
        }
        else if(proc == 0)
        {
            printf("Execute process %s\n", arguments[0]);
            makelimits(argv[2], argv[3]);
            execvp(arguments[0], arguments);
        }
        else
        {
            int status;
            waitpid(proc, &status, 0);
            if(WEXITSTATUS(status))
            {
                printf("Error while executing %s\n", arguments[0]);
                continue;
            }
            if(WIFSIGNALED(status))
            {
                printf("Process %s terminated because of limit exceed\n", arguments[0]);
                continue;
            }
        }

        struct rusage usage;
        getrusage(RUSAGE_CHILDREN, &usage);

        struct timeval ru_utime;
        struct timeval ru_stime;

        timersub(&usage.ru_utime, &prev_usage.ru_utime, &ru_utime);
        timersub(&usage.ru_stime, &prev_usage.ru_stime, &ru_stime);

        prev_usage = usage;
        printf("\nProcess %s: User CPU time used: %d.%d seconds,  system CPU time used: %d.%d seconds\n\n", arguments[0], (int) ru_utime.tv_sec,
                (int) ru_utime.tv_usec, (int) ru_stime.tv_sec, (int) ru_stime.tv_usec);
    }

    free(line);
    fclose(file);

    return 0;
}
