#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <zconf.h>
#include <signal.h>
#include <semaphore.h>
/**********************************************/
char** buffer = NULL;
int N = 0;
int P = 0;
int K = 0;
int L = 0;
int lastWrite = 0;
int lastRead = 0;
int searchMode = 1;
int raportMode = 1;
int bufferLines = 0;
/**********************************************/
FILE *product;
/**********************************************/
sem_t bufferSem;
sem_t emptyBufferSem;
sem_t fullBufferSem;
/**********************************************/
void *producer(void)
{
    char *line = NULL;
    size_t lineSize = 0;

    while(1)
    {
        sem_wait(&fullBufferSem);
        sem_wait(&bufferSem);

        if(getline(&line, &lineSize, product) <= 0)
        {
            sem_post(&bufferSem);
            break;
        }

        lastWrite = (lastWrite + 1) % N;
        buffer[lastWrite] = malloc(lineSize * sizeof(char));
        strcpy(buffer[lastWrite], line);
        bufferLines++;

        if(raportMode == 1)
        {
            printf("Producer is putting line at %i, of length %i. It is the %i line in buffer.\n", lastWrite, (int)lineSize, bufferLines);
        }

        sem_post(&emptyBufferSem);
        sem_post(&bufferSem);
    }
}
/**********************************************/
void *consumer(void)
{
    char *line;

    while(1)
    {
        sem_wait(&emptyBufferSem);
        sem_wait(&bufferSem);

        lastRead = (lastRead + 1) % N;
        line = buffer[lastRead];

        bufferLines--;

        if(raportMode == 1)
        {
            printf("Consumer is getting line from %i. It is the %i line in buffer.\n", lastRead, bufferLines);
        }

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
        sem_post(&fullBufferSem);
        sem_post(&bufferSem);
    }
}
/**********************************************/
void destroy()
{
    fclose(product);
    free(buffer);
    sem_destroy(&bufferSem);
    sem_destroy(&emptyBufferSem);
    sem_destroy(&fullBufferSem);
}
/**********************************************/
void sigExit(int signum)
{
    printf("Ending program\n");
    exit(EXIT_SUCCESS);
}
/**********************************************/
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

    sem_init(&bufferSem, 0, 1);
    sem_init(&fullBufferSem, 0, N);
    sem_init(&emptyBufferSem, 0, 0);

    for(int i = 0 ; i < N ; i++)
    {
        buffer[i] = NULL;
    }

    P = atoi(argv[2]);
    K = atoi(argv[3]);
    L = atoi(argv[4]);

    searchMode = atoi(argv[5]);
    char* file = argv[6];
    if ((product = fopen(file, "r")) < 0)
    {
        printf("Error during opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int nk = atoi(argv[7]);

    raportMode = atoi(argv[8]);

    pthread_t producers[P];
    pthread_t consumers[K];

    for(int i = 0 ; i < P ; i++)
    {
        pthread_create(&producers[i], NULL , &producer, NULL);
    }
    for(int i = 0 ; i < K ; i++)
    {
        pthread_create(&consumers[i], NULL , &consumer, NULL);
    }
    for(int i = 0 ; i < P ; i++) pthread_join(producers[i], NULL);

    while (1)
    {
        sem_wait(&bufferSem);
        if (bufferLines == 0) break;
        sem_post(&bufferSem);
    }

    exit(EXIT_SUCCESS);
}