#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
/***********************************************************/
int sentSignals = 0;
int recSignals = 0;
int recSignalsP = 0;

int amount;
int type;

pid_t mainPid;
pid_t child;
/***********************************************************/
void sigParent(int signum)
{
    if(signum == SIGUSR1 || signum == SIGRTMIN)
    {
        printf("Proces macierzysty odbiera sygnal od potomka nr %i\n", ++recSignals);
    }
    else if(signum == SIGINT)
    {
        kill(child, (type != 3 ? SIGUSR2 : SIGRTMAX));
        exit(EXIT_SUCCESS);
    }
}
/***********************************************************/
void sigChild(int signum)
{
    if(signum == SIGUSR1 || signum == SIGRTMIN)
    {
        printf("Potomek odbiera sygnal nr %i\n", ++recSignalsP);
        kill(getppid(), (type != 3 ? SIGUSR1 : SIGRTMIN));
    }
    if(signum == SIGUSR2 || signum == SIGRTMAX)
    {
        printf("Odebranych sygnalow przez potomka: %i\n", recSignalsP);
        printf("Potomek odbiera sygnal konczacy prace\n");
        exit(EXIT_SUCCESS);
    }
}
/***********************************************************/
void functionChild(int Type)
{
    type = Type;
    struct sigaction childAct;
    childAct.sa_sigaction = sigChild;
    sigfillset(&childAct.sa_mask);
    sigaddset(&childAct.sa_mask, SIGUSR1);
    childAct.sa_flags = 0;
    if(sigaction(SIGINT, &childAct, NULL) < 0 || sigaction((type != 3 ? SIGUSR1 : SIGRTMIN), &childAct, NULL) < 0
       || sigaction((type != 3 ? SIGUSR2 : SIGRTMAX), &childAct, NULL) < 0)
    {
        printf("Blad podczas rejestrowania obslugi sygnalow dla potomka\n");
        exit(EXIT_FAILURE);
    }

    sigset_t newmask;
    sigset_t oldmask;
    sigfillset(&newmask);
    sigdelset(&newmask, SIGINT);
    sigdelset(&newmask, SIGUSR1);
    sigdelset(&newmask, SIGUSR2);
    sigdelset(&newmask, SIGRTMIN);
    sigdelset(&newmask, SIGRTMAX);
    if(sigprocmask(SIG_SETMASK, &newmask, &oldmask) < 0)
    {
        printf("Blad podczas ustawiania maski\n");
        exit(EXIT_FAILURE);
    }

    while(1) pause();
}
/***********************************************************/
void functionParent()
{
    struct sigaction parentAct;
    parentAct.sa_sigaction = sigParent;
    sigfillset(&parentAct.sa_mask);
    parentAct.sa_flags = 0;
    if(sigaction(SIGINT, &parentAct, NULL) < 0 || sigaction(type != 3 ? SIGUSR1 : SIGRTMIN, &parentAct, NULL) < 0)
    {
        printf("Blad podczas rejestrowania obslugi sygnalow przodka\n");
        kill(child, SIGINT);
        exit(EXIT_FAILURE);
    }

    for(int i = 0 ; i < amount ; i++)
    {
        printf("Proces macierzysty wysyla sygnal nr %i\n", ++sentSignals);
        kill(child, (type != 3 ? SIGUSR1 : SIGRTMIN));
        if(type == 2) pause();
    }

    int maxWait = 10;
    while(recSignals < amount && maxWait > 0)
    {
        sleep(1);
        maxWait--;
    }
    printf("Proces macierzysty wysyla sygnal konczacy prace\n");
    kill(child, (type != 3 ? SIGUSR2 : SIGRTMAX));
    printf("Wyslanych sygnalow przez proces macierzysty: %i\n", sentSignals);
    printf("Odebranych sygnalow przez proces macierzysty: %i\n", recSignals);
    exit(EXIT_SUCCESS);
}
/***********************************************************/
int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("Zla liczba argumentow. Podaj ilosc sygnalow i typ (1,2,3)\n");
        exit(EXIT_FAILURE);
    }
    if(atoi(argv[1]) <= 0 || atoi(argv[2]) < 1 || atoi(argv[2]) > 3)
    {
        printf("Bledne argumenty.\n");
        exit(EXIT_FAILURE);
    }
    mainPid = getpid();
    amount = atoi(argv[1]);
    type = atoi(argv[2]);


    if((child = fork()) < 0)
    {
        printf("Blad podczas tworzenia procesu potomnego\n");
        exit(EXIT_FAILURE);
    }
    else if(child == 0)
    {
        functionChild(type);
    }
    else
    {
        printf("Proces potomny utworzony\n");
        functionParent();
    }
    return 0;
}
