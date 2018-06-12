#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
/****************************************************/
FILE *input;
FILE *output;
FILE *filter;
/****************************************************/
struct Arg
{
    float **filter;
    float **input;
    float **output;
    int c;
    int W;
    int H;
    int start;
    int end;
};
/****************************************************/
void timeDifference(struct timeval t1, struct timeval t2)
{
    long seconds = t2.tv_sec - t1.tv_sec;
    long mseconds = t2.tv_usec - t1.tv_usec;
    if (mseconds < 0)
    {
        mseconds += 1000000;
        seconds -= 1;
    }
    printf("Time lasted: %ld.%06lds\n", seconds, mseconds);
}
/****************************************************/
int findSuitable(int min, int m, int max)
{
    if(m <= min) return min;
    else if (m > min && m <= max) return m;
    else return max;
}
/****************************************************/
int calculate(struct Arg *arg, int x, int y)
{
    float pixel = 0;
    int filterX = 0, filterY = 0, suitI, suitJ;

    for(int i = y - (arg->c / 2) ; i <= y + (arg->c / 2) ; i++, filterY++)
    {
        filterX = 0;
        for(int j = x - (arg->c / 2) ; j <= x + (arg->c / 2) ; j++, filterX++)
        {
            suitI = findSuitable(0, i, (arg->H - 1));
            suitJ = findSuitable(0, j, (arg->W - 1));
            pixel += arg->input[suitI][suitJ] * arg->filter[filterY][filterX];
        }
    }
    //printf("x y %d %d\n", x, y);
    //printf("p: %d\n", (int)roundf(pixel));
    return (int)roundf(pixel);
}
/****************************************************/
void *fillTable(void *arg)
{
    struct Arg *argument = arg;
    for(int i = argument->start ; i < argument->end; i++)
    {
        for(int j = 0 ; j < argument->W ; j++)
        {
            argument->output[i][j] = calculate(argument, j, i);
        }
    }

    pthread_exit(NULL);
}
/****************************************************/
int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    int threads = strtol(argv[1], NULL, 10);
    input = fopen(argv[2], "r");
    filter = fopen(argv[3], "r");
    output = fopen(argv[4], "w");

    if(input == NULL || output == NULL || filter == NULL)
    {
        printf("Problem with files.\n");
        exit(EXIT_FAILURE);
    }

    char line[1000];

    //loading filter
    fgets(line, 1000, filter);
    int c = strtol(line, NULL, 10);
    float **filterTable = malloc(c * sizeof(float *));
    for(int i = 0 ; i < c ; i++)
    {
        filterTable[i] = malloc(c * sizeof(float));
    }

    int i = 0, j = 0;
    char *one;
    while(fgets(line, 1000, filter))
    {
        j = 0;
        one = line;
        while(strtok_r(NULL, " \n", &one))
        {
            filterTable[i][j++] = strtof(one, NULL);
        }
        i++;
    }

    //loading input
    fgets(line, 1000, input);
    fgets(line, 1000, input);
    one = line;
    int W = strtol(strtok_r(NULL, " \n", &one), NULL, 10);
    int H = strtol(strtok_r(NULL, " \n", &one), NULL, 10);
    fgets(line, 1000, input);
    one = line;
    int M = strtol(strtok_r(NULL, " \n", &one), NULL, 10);

    float **inputTable = malloc(H * sizeof(float *));
    for(i = 0 ; i < H ; i++)
    {
        inputTable[i] = malloc(W * sizeof(float));
    }

    i = 0, j = 0;
    while(fgets(line, 1000, input))
    {
        one = line;
        char *check;
        while(check = strtok_r(NULL, " \n", &one))
        {
            if(j >= W)
            {
                i++;
                j = 0;
            }
            inputTable[i][j++] = strtol(check, NULL, 10);
        }
    }

    float **out = malloc(H * sizeof(float *));
    for(i = 0 ; i < H ; i++)
    {
        out[i] = malloc(W * sizeof(float));
    }

    pthread_t threadTable[threads];
    int threadsCounter = threads;
    int rowsLeft = H + 1;
    int start = 0;
    int end = start + rowsLeft / threadsCounter;
    int amount = 0;
    i = 0;
    struct Arg argTable[threads];

    while(threadsCounter > 0)
    {
        struct Arg arg;
        arg.filter = filterTable;
        arg.input = inputTable;
        arg.output = out;
        arg.c = c;
        arg.W = W;
        arg.H = H;
        arg.start = start;
        if(threads != 1) arg.end = end;
        else arg.end = H;

        argTable[i] = arg;
        pthread_t thread;
        threadTable[i++] = thread;

        amount = rowsLeft / threadsCounter;
        rowsLeft -= amount;
        start = end;
        threadsCounter--;

        if(threadsCounter == 1) end = H;
        else end = start + amount;
    }

    struct timeval t1, t2;
    gettimeofday(&t1, 0);

    for(i = 0 ; i < threads ; i++)
    {
        if(pthread_create(&threadTable[i], NULL, &fillTable, &argTable[i]))
        {
            printf("Cannot make a thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    for(i = 0; i < threads; i++)
    {
        pthread_join(threadTable[i], NULL);
    }

    gettimeofday(&t2, 0);
    timeDifference(t1, t2);

    fprintf(output, "P2\n");
    fprintf(output, "%d %d\n", W, H);
    fprintf(output, "%d\n", M);

    for(i = 0 ; i < H ; i++)
    {
        for(j = 0 ; j < W ; j++)
        {
            fprintf(output, "%d ", (int)out[i][j]);
        }
        fprintf(output, "\n");
    }

    fclose(input);
    fclose(filter);
    fclose(output);

    exit(EXIT_SUCCESS);
}