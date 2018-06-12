#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/un.h>
/*********************************************/
#include "common.h"
/*********************************************/
int inet, local, epolld;
int expressionCounter;
char *localName;
/*********************************************/
pthread_mutex_t clientsMutex;
struct Client clients[MAXCLIENTS];
/*********************************************/
void clean()
{
    close(epolld);
    close(inet);
    close(local);
    remove(localName);
}
/*********************************************/
void sigInt(int signum)
{
    if(signum == SIGINT)
    {
        printf("Got SIGINT signal. Exiting.\n");
        exit(EXIT_SUCCESS);
    }
}
/*********************************************/
void closeConnection(struct epoll_event events)
{
    pthread_mutex_lock(&clientsMutex);
    shutdown(events.data.fd, SHUT_RDWR);
    close(events.data.fd);

    for(int i = 0 ; i < MAXCLIENTS ; i++)
    {
        if(clients[i].pings > 0 && events.data.fd == clients[i].fd)
        {
            clients[i].pings = -1;
            for(int j = 0 ; j < MAXLENGTH ; j++) clients[i].name[j] = 0;
        }
    }

    pthread_mutex_unlock(&clientsMutex);
}
/*********************************************/
void handleResponse(struct epoll_event events)
{
    struct Msg msg;
    struct sockaddr *addr = malloc(sizeof(struct sockaddr));
    socklen_t addrSize = sizeof(struct sockaddr);
    ssize_t quantity = recvfrom(events.data.fd, &msg, sizeof(msg), 0, addr, &addrSize);
    if (quantity == 0)
    {
        printf("Ending connection with client %d.\n", events.data.fd);
        fflush(stdout);
        closeConnection(events);
        return;
    }
    else
    {
        switch(msg.msgType)
        {
            case START:
                pthread_mutex_lock(&clientsMutex);
                for(int i = 0 ; i < MAXCLIENTS ; i++)
                {
                    if(clients[i].pings >= 0 && strcmp(msg.name, clients[i].name) == 0)
                    {
                        pthread_mutex_unlock(&clientsMutex);
                        sendto(events.data.fd, &msg, sizeof(msg), 0, &clients[i].addr, clients[i].addrSize);
                        closeConnection(events);
                        return;
                    }
                }
                for(int i = 0 ; i < MAXCLIENTS ; i++)
                {
                    if(clients[i].pings >= 0 && events.data.fd == clients[i].fd)
                    {
                        printf("Starting connection with client %d - %s.\n", events.data.fd, msg.name);
                        fflush(stdout);
                        strcpy(clients[i].name, msg.name);
                        clients[i].addr = *addr;
                        clients[i].addrSize = addrSize;
                        clients[i].fd = events.data.fd;
                        clients[i].pings =0;
                        pthread_mutex_unlock(&clientsMutex);
                        return;
                    }
                }
            case ORDER:
                expressionCounter++;
                printf("Expression %d, from client %s. Result: %d.\n", expressionCounter, msg.name, msg.msgOrder.operation);
                fflush(stdout);
                return;
            case PING:
                pthread_mutex_lock(&clientsMutex);
                clients[msg.id].pings = 0;
                pthread_mutex_unlock(&clientsMutex);
                return;
            default:
                break;
        }
    }
}
/*********************************************/
void *math(void *args)
{
    for(int i = 0 ; i < MAXCLIENTS ; i++) clients[i].pings = -1;
    struct epoll_event events[MAXEVENTS];

    while(1)
    {
        int counter = epoll_wait(epolld, events, MAXEVENTS, -1);
        for(int i = 0 ; i < counter ; i++)
        {
            handleResponse(events[i]);
        }
    }
}
/*********************************************/
void *ping(void *args)
{
    sleep(5);

    while(1)
    {
        struct Msg msg;
        msg.msgType = PING;
        pthread_mutex_lock(&clientsMutex);
        for(int i = 0 ; i < MAXCLIENTS ; i++)
        {
            if(clients[i].pings == 0)
            {
                clients[i].pings = 1;
                msg.id = i;
                sendto(clients[i].fd, &msg, sizeof(msg), 0, &clients[i].addr, clients[i].addrSize);
            }
        }

        sleep(5);

        for(int i = 0 ; i < MAXCLIENTS ; i++)
        {
            if(clients[i].pings == 1)
            {
                clients[i].pings = -1;
                for(int j = 0 ; j < MAXLENGTH ; j++)
                {
                    clients[i].name[j] = 0;
                }
            }
        }
        pthread_mutex_unlock(&clientsMutex);
        sleep(10);
    }
}
/*********************************************/
void mainProcess();
/*********************************************/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    atexit(clean);
    signal(SIGINT, sigInt);

    pthread_mutex_init(&clientsMutex, NULL);

    unsigned short port = (unsigned short)strtoul(argv[1], NULL, 10);
    localName = argv[2];

    inet = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(inet, (struct sockaddr*)&(addr), sizeof(addr)) != 0)
    {
        printf("Cannot bind in INET.\n");
        exit(EXIT_FAILURE);
    }

    epolld = epoll_create1(0);
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = inet;

    if(epoll_ctl(epolld, EPOLL_CTL_ADD, inet, &event) < 0)
    {
        printf("Cannot start epoll for INET.\n");
        exit(EXIT_FAILURE);
    }

    local = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, localName);

    if(bind(local, (struct sockaddr*)&(address), sizeof(address)) != 0)
    {
        printf("Cannot bind in LOCAL.\n");
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = local;

    if(epoll_ctl(epolld, EPOLL_CTL_ADD, local, &event) < 0)
    {
        printf("Cannot start epoll for LOCAL.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t mathThread;
    pthread_create(&mathThread, NULL, math, NULL);

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, ping, NULL);

    mainProcess();

    exit(EXIT_SUCCESS);
}
/*********************************************/
void mainProcess()
{
    int operation;
    int operand1;
    int operand2;

    sleep(5);

    while(1)
    {
        printf("Type operation and two operands.\n");
        printf("ADD - 1, SUB - 2, MUL - 3, DIV - 4:\n");

        int counter = scanf("%d %d %d", &operation, &operand1, &operand2);
        if(counter != 3)
        {
            printf("Type operation: ");
            char c;
            while((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        struct Msg msg;
        msg.msgType = ORDER;
        msg.msgOrder.operation = operation;
        msg.msgOrder.operand1 = operand1;
        msg.msgOrder.operand2 = operand2;

        int loop = 1;

        while(loop)
        {
            pthread_mutex_lock(&clientsMutex);
            for(int i = 0 ; i < MAXCLIENTS ; i++)
            {
                if(clients[i].pings == 0)
                {
                    ssize_t quantity = sendto(clients[i].fd, &msg, sizeof(msg), 0, &clients[i].addr, clients[i].addrSize);
                    if (quantity <= 0)
                    {
                        printf("Cannot send message to client %d.\n", clients[i].fd);
                        fflush(stdout);
                    }
                    else
                    {
                        printf("Sending order.\n");
                        fflush(stdout);
                        loop = 0;
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&clientsMutex);
        }
    }
}
