#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <zconf.h>
#include <bits/signum.h>
#include <signal.h>
/******************************************************/
char **buffer = NULL;
int N = 0;
int P = 0;
int K = 0;
int L = 0;
int searchMode = 1;
int raportMode = 1;
int lastWrite = 0;
int lastRead = 0;
int bufferLines = 0;
/******************************************************/
FILE *product;
/******************************************************/
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t emptyBuffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t fullBuffer = PTHREAD_COND_INITIALIZER;
/******************************************************/
void *producer(void)
{
    char* line = NULL;
    size_t lineSize = 0;
    while(1)
    {
        pthread_mutex_lock(&bufferMutex);
        while(bufferLines == N)
        {
            pthread_cond_wait(&fullBuffer, &bufferMutex);
        }

        if(getline(&line, &lineSize, product) <= 0)
        {
            pthread_mutex_unlock(&bufferMutex);
            break;
        }

        lastWrite = (lastWrite + 1) % N;
        buffer[lastWrite] = malloc(lineSize * sizeof(char));
        strcpy(buffer[lastWrite], line);
        bufferLines++;

        if(raportMode == 1) {
            printf("Producer is putting line at %i, of length %i. It is the %i line in buffer.\n", lastWrite, (int)lineSize, bufferLines);
        }

        if(bufferLines == 1) pthread_cond_broadcast(&emptyBuffer);
        pthread_mutex_unlock(&bufferMutex);
    }
}
/******************************************************/
void *consumer(void)
{
    char *line;
    while(1)
    {
        pthread_mutex_lock(&bufferMutex);
        while(bufferLines == 0)
        {
            pthread_cond_wait(&emptyBuffer, &bufferMutex);
        }

        lastRead = (lastRead + 1) % N;
        line = buffer[lastRead];
        bufferLines--;

        if(bufferLines == N - 1) pthread_cond_broadcast(&fullBuffer);

        printf("Consumer is getting line from %i. It is %i line in buffer.\n", lastRead, bufferLines);

        if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        int flag;
        switch(searchMode)
        {
            case 0:
                flag = (strlen(line) == L);
                break;
            case 1:
                flag = (strlen(line) > L);
                break;
            case -1:
                flag = (strlen(line) < L);
                break;
        }
        if(flag) printf("Index: %i, line %s \n", lastRead, line);
        free(line);
        pthread_mutex_unlock(&bufferMutex);
    }
}
/******************************************************/
void destroy()
{
    fclose(product);
    free(buffer);
    pthread_mutex_destroy(&bufferMutex);
    pthread_cond_destroy(&emptyBuffer);
    pthread_cond_destroy(&fullBuffer);
}
/******************************************************/
void sigExit(int signum)
{
    printf("Ending program\n");
    exit(EXIT_SUCCESS);
}
/******************************************************/
int main(int argc, char *argv[])
{
    atexit(&destroy);
    signal(SIGINT, sigExit);

    if(argc < 9)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    buffer = malloc(sizeof(char*) * N);

    for(int i = 0 ; i < N ; i++)
    {
        buffer[i] = NULL;
    }

    P = atoi(argv[2]);
    K = atoi(argv[3]);
    L = atoi(argv[4]);
    searchMode = atoi(argv[5]);
    char *file = argv[6];

    if((product = fopen(file, "r")) < 0)
    {
        printf("Error during opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int nk = atoi(argv[7]);

    raportMode = atoi(argv[8]);

    pthread_t producents[P];
    pthread_t consumers[K];

    for(int i = 0 ; i < P ; i++)
    {
        pthread_create(&producents[i], NULL , &producer, NULL);
    }
    for(int i = 0 ; i < K ; i++)
    {
        pthread_create(&consumers[i], NULL , &consumer, NULL);
    }
    for(int i = 0 ; i < P ; i++) pthread_join(producents[i], NULL);

    while (1)
    {
        pthread_mutex_lock(&bufferMutex);
        if (bufferLines == 0) break;
        pthread_mutex_unlock(&bufferMutex);
    }

    exit(EXIT_SUCCESS);
}
