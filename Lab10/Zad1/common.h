#ifndef CLION1_COMMON_H
#define CLION1_COMMON_H
/*********************************************/
#define MAXCLIENTS 15
#define MAXLENGTH 100
#define MAXEVENTS 10
/*********************************************/
#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
/*********************************************/
#define INET 5
#define LOCAL 6
#define START 7
#define ORDER 8
#define PING 9
/*********************************************/
struct Client
{
    char name[MAXLENGTH];
    int fd;
    int pings;
};
/*********************************************/
struct Order
{
    int operation;
    int operand1;
    int operand2;
};
/*********************************************/
struct Msg
{
    int id;
    int msgType;
    char name[MAXLENGTH];
    struct Order msgOrder;
};
/*********************************************/
#endif //CLION1_COMMON_H
