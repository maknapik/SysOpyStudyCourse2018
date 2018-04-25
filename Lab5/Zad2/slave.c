#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
/********************************************************/
const int bufSize = 4096;
/********************************************************/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)(time(NULL) * getpid()));

    int FIFO = open(argv[1], O_WRONLY);
    if(FIFO < 0)
    {
        printf("Error during opening FIFO\n");
        exit(EXIT_FAILURE);
    }

    char buf[2][bufSize];
    FILE *date;

    for(int i = 0 ; i < atoi(argv[2]) ; i++)
    {
        date = popen("date", "r");
        fgets(buf[0], bufSize, date);
        sprintf(buf[1], "Slave: %d, date: %s", getpid(), buf[0]);
        write(FIFO, buf[1], strlen(buf[1]));
        fclose(date);
        sleep((unsigned int) (rand() % 4 + 2));
    }

    close(FIFO);
    exit(EXIT_SUCCESS);
}