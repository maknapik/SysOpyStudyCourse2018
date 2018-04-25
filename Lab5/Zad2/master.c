#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
/********************************************************/
const int bufSize = 4096;
/********************************************************/
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    if(mkfifo(argv[1], S_IRUSR | S_IWUSR) == -1)
    {
        printf("Error during creating pipe\n");
        exit(EXIT_FAILURE);
    }

    FILE *FIFO = fopen(argv[1], "r");
    if(!FIFO)
    {
        printf("Error during opening FIFO\n");
        exit(EXIT_FAILURE);
    }

    char buf[bufSize];
    while(fgets(buf, bufSize, FIFO))
    {
        printf("Buffer: %s\n", buf);
    }

    fclose(FIFO);
    exit(EXIT_SUCCESS);
}