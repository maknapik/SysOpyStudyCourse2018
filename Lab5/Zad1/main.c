#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
/*****************************************************************/
const int maxCommands = 50;
const int maxLines = 50;
/*****************************************************************/
char *removeSpaces(char *str)
{
    char *buf = malloc(sizeof(char) * 100);
    char *s = str;

    while(*s == ' ') s++;

    int i = 0;
    while(*s != 0)
    {
        while(*s != ' ' && *s != 0)
        {
            buf[i++] = *s;
            s++;
        }
        if(*s == ' ')
        {
            while(*s == ' ') s++;
            if(*s != 0) buf[i++] = ' ';
        }
    }
    buf[i + 1] = 0;
    return buf;
}
/*****************************************************************/
char **parseArguments(char *line)
{
    int size = 0;
    char **arguments = NULL;
    const char delim[2] = {' ', '\n'};
    char *s = strtok(line, delim);
    while(s != NULL)
    {
        size++;
        arguments = realloc(arguments, sizeof(char*) * size);
        if(arguments == NULL)
        {
            exit(EXIT_FAILURE);
        }
        arguments[size - 1] = s;
        s = strtok(NULL, delim);
    }

    arguments = realloc(arguments, sizeof(char*) * (size + 1));
    if(arguments == NULL)
    {
        exit(EXIT_FAILURE);
    }
    arguments[size] = NULL;

    return arguments;
}
/*****************************************************************/
int execLine(char *params)
{
    int cmdNumber = 0;
    int pipes[2][2];
    char *commands[maxCommands];


    while((commands[cmdNumber] = strtok(cmdNumber == 0 ? params : NULL, "|")) != NULL) cmdNumber++;

    int i;
    for(i = 0 ; i < cmdNumber ; i++)
    {
        if(i > 0)
        {
            close(pipes[i % 2][0]);
            close(pipes[i % 2][1]);
        }

        if(pipe(pipes[i % 2]) == -1)
        {
            printf("Pipe error\n");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if(pid == 0)
        {
            char ** execParams = parseArguments(removeSpaces(commands[i]));

            if(i != cmdNumber - 1)
            {
                close(pipes[i % 2][0]);
                if (dup2(pipes[i % 2][1], STDOUT_FILENO) < 0)
                {
                    exit(EXIT_FAILURE);
                }
            }
            if(i != 0)
            {
                close(pipes[(i + 1) % 2][1]);
                if (dup2(pipes[(i + 1) % 2][0], STDIN_FILENO) < 0)
                {
                    close(EXIT_FAILURE);
                }
            }
            execvp(execParams[0], execParams);

            exit(EXIT_SUCCESS);
        }
    }

    close(pipes[i % 2][0]);
    close(pipes[i % 2][1]);

    wait(NULL);
    exit(EXIT_SUCCESS);
}
/*****************************************************************/
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Wrong number of arguments. Type name of file.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");
    if(!file)
    {
        printf("Error during opening the file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char line[maxLines];
    while(fgets(line, maxLines, file))
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            execLine(line);
            exit(EXIT_SUCCESS);
        }

        int status;
        wait(&status);
        if(status)
        {
            printf("Error during executing line\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
    exit(EXIT_SUCCESS);
}

