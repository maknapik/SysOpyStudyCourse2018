#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
    if(argc != 2)
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
            execvp(arguments[0], arguments);
        }
        else
        {
            int status;
            waitpid(proc, &status, 0);
            if(WEXITSTATUS(status)) printf("Error while executing %s\n", arguments[0]);
        }
    }

    free(line);
    fclose(file);

    return 0;
}
