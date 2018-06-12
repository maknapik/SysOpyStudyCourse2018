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
char *name;
char *address;
unsigned short port;
int type;
int socketd;
/*********************************************/
int calculate(int operation, int operand1, int operand2)
{
    if (operation == ADD)
    {
        return operand1 + operand2;
    }
    else if (operation == SUB)
    {
        return operand1 - operand2;
    }
    else if (operation == MUL)
    {
        return operand1 * operand2;
    }
    else if (operation == DIV)
    {
        if (operand2 == 0)
        {
            printf("Error during div operation. Giving -1 as result.\n");
            fflush(stdout);
            return -1;
        }
        return operand1 / operand2;
    }
    else
    {
        printf("Wrong expression. Giving -1 as a result.\n");
        fflush(stdout);
        return -1;
    }
}
/*********************************************/
void clean()
{
    shutdown(socketd, SHUT_RDWR);
    close(socketd);
}
/*********************************************/
void sigInt(int signum)
{
    if (signum == SIGINT) {
        printf("Got SIGINT signal. Exiting.\n");
        exit(EXIT_SUCCESS);
    }
}
/*********************************************/
void responseToOrder(struct Order order, int id)
{
    struct Msg msg;
    strcpy(msg.name, name);
    msg.msgType = ORDER;
    msg.id = id;
    int result = calculate(order.operation, order.operand1, order.operand2);
    msg.msgOrder.operation = result;
    printf("Client: %s calculated expression and sends result: %d to server.\n", msg.name, msg.msgOrder.operation);
    fflush(stdout);
    if(write(socketd, &msg, sizeof(msg)) < 0)
    {
        printf("Cannot send calculated result to server in client: %s.\n", msg.name);
        exit(EXIT_FAILURE);
    }
}
/*********************************************/
void mainProcess();
/*********************************************/
int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        printf("Wrong number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    name = argv[1];
    type = strtol(argv[2], NULL, 10);
    address = argv[3];

    if(type == INET)
    {
        port = strtoul(argv[4], NULL, 10);
        socketd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketd < 0)
        {
            printf("Cannot create INET socket.\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        if(inet_pton(AF_INET, address, &addr.sin_addr) == 0)
        {
            printf("Cannot pton in INET.\n");
            exit(EXIT_FAILURE);
        }

        addr.sin_port = htons(port);
        if(connect(socketd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            printf("Cannot connect to INET.\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(type == LOCAL)
    {
        socketd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if(socketd < 0)
        {
            printf("Cannot create LOCAL socket.\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, address);
        if(bind(socketd, (const struct sockaddr*)&addr, sizeof(sa_family_t)) < 0)
        {
            printf("Cannot bind LOCAL.\n");
            exit(EXIT_FAILURE);
        }

        if(connect(socketd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            printf("Cannot connect to LOCAL.\n");
            exit(EXIT_FAILURE);
        }
    }

    atexit(clean);
    signal(SIGINT, sigInt);

    struct Msg msg;
    strcpy(msg.name, name);
    msg.msgType = START;
    if(write(socketd, &msg, sizeof(msg)) < 0)
    {
        printf("Client: %s cannot start connection.\n", name);
        exit(EXIT_FAILURE);
    }

    printf("Client %s connected.\n", name);
    fflush(stdout);

    mainProcess();

    exit(EXIT_SUCCESS);
}
/*********************************************/
void mainProcess()
{
    while(1)
    {
        struct Msg msg;
        ssize_t quantity = recv(socketd, &msg, sizeof(msg), MSG_WAITALL);
        if(quantity == 0)
        {
            printf("Server is stopped.\n");
            exit(EXIT_FAILURE);
        }

        switch(msg.msgType)
        {
            case START:
                printf("Client with that name already exists.\n");
                exit(EXIT_FAILURE);
            case ORDER:
                fflush(stdout);
                responseToOrder(msg.msgOrder, msg.id);
                break;
            case PING:
                printf("Client pinged by server.\n");
                fflush(stdout);
                if(write(socketd, &msg, sizeof(msg)) < 0)
                {
                    printf("Cannot send ping to server in client: %s.\n", name);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printf("Unrecognized operation from server: %d.\n", msg.msgType);
                fflush(stdout);
                break;
        }
    }
}
