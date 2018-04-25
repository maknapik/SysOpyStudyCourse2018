#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
/*********************************************************************/
pid_t childPid;
/*********************************************************************/
void sigInt(int signum) //obsługa SIGINT
{
    printf("Odebrano sygnal SIGINT\n");
    if(childPid == 0) return; //jesli jestesmy w procesie potomnym
    kill(childPid, SIGKILL);
    exit(EXIT_SUCCESS);
}
/*********************************************************************/
void sigTstp(int signum) //obsługa SIGTSTP
{
    if(kill(childPid, 0) < 0)
    {
        if(errno == ESRCH)
        {
            childPid = fork();
            if(childPid < 0)
            {
                printf("Blad podczas tworzenia procesu potomnego\n");
                exit(EXIT_FAILURE);
            }
            if(childPid == 0)
            {
                sigset_t newMask, oldMask;
                sigemptyset(&newMask);
                sigaddset(&newMask, SIGINT);
                sigprocmask(SIG_BLOCK, &newMask, &oldMask);

                char* date[] = {"./script.sh",NULL};
                execvp("./script.sh",date);
                exit(EXIT_SUCCESS);
            }
        }
    }
    else
    {
        if(kill(childPid, 9) == 0)
        {
            int stat;
            waitpid(childPid, &stat, 0);
        }
        printf("Oczekuje na CTRL + Z - kontynuacja, albo CTR + C - zakonczenie programu\n");
    }
}
/*********************************************************************/
int main()
{
    if(signal(SIGINT, sigInt) == SIG_ERR)
    {
        printf("Blad podczas rejestrowania obslugi sygnalu SIGINT\n");
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_handler = sigTstp;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(sigaction(SIGTSTP, &act, NULL) == -1)
    {
        printf("Blad podczas rejestrowania obslugi sygnalu SIGTSTP\n");
        exit(EXIT_FAILURE);
    }

    childPid = fork();
    if(childPid < 0)
    {
        printf("Blad podczas tworzenia procesu potomnego\n");
        exit(EXIT_FAILURE);
    }
    if(childPid == 0)
    {
        sigset_t newMask, oldMask;
        sigemptyset(&newMask);
        sigaddset(&newMask, SIGINT);
        sigprocmask(SIG_BLOCK, &newMask, &oldMask);

        char* date[] = {"./script.sh",NULL};
        execvp("./script.sh",date);
        exit(EXIT_SUCCESS);
    }

    while(1) pause();

    return 0;
}
