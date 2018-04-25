#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
/*******************************************************/
int processes = 0;
int rtSignals = 0;
int N, K;
int *pendingProcesses;
int *mask;
int enabled = 0;
int chEnabled = 0;
int maxSleep = 15;
int mainPid;
/*******************************************************/
void sigInt(int signum)
{
    for(int i = 0 ; i < processes ; i++)
    {
        kill(pendingProcesses[i], SIGINT);
    }
    exit(EXIT_SUCCESS);
}
/*******************************************************/
void sigUsr1(int signum, siginfo_t *info, void *c)
{
    if(getpid() != mainPid)
    {
        if(mask[2] == 1 && chEnabled == 0) printf("Wyslano pozwolenie do procesu PID: %i\n", getpid());
        chEnabled = 1;
        return;
    }

    if(mask[1] == 1) printf("Otrzymano %i prosbe od procesu PID: %i\n", processes + 1, info->si_pid);

    processes++;
    pendingProcesses[processes-1] = info->si_pid;
    if(processes < K)
    {
        return;
    }
    else
    {
        if(enabled == 1) kill(pendingProcesses[processes-1], SIGUSR1);
        else
        {
            enabled = 1;
            for(int i = 0 ; i < processes ; i++)
            {
                kill(pendingProcesses[i], SIGUSR1);
            }
        }
    }
}
/*******************************************************/
void sigRt(int signum, siginfo_t *info, void *c)
{
    if(mask[3] == 1) printf("Otrzymano %i sygnal czasu rzeczywistego SIGRT%i od procesu PID: %d\n", (rtSignals++) + 1, info->si_signo-SIGRTMIN, info->si_pid);
}
/*******************************************************/
void setMask(int *tab) {
    printf("Wpisz sekwencje 5 bitow, wedlug opcji ponizej: \n");
    printf("1.Tworzenie procesu potomnego (jego nr PID)\n");
    printf("2.Otrzymane prosby od procesow potomnych\n");
    printf("3.Wyslane pozwolenia na wyslanie sygnalu rzeczywistego\n");
    printf("4.Otrzymane sygnaly czasu rzeczywistego (wraz z numerem sygnalu)\n");
    printf("5.Zakonczenie procesu potomnego (wraz ze zwroconą wartoscia)\n");

    char *tmp1 = (char*)calloc(6, sizeof(char));
    scanf("%s", tmp1);
    int tmp2 = (int)strtol(tmp1, NULL, 10);
    for (int i = 4; i >= 0; --i)
    {
        int tmp3 = tmp2 % 10;
        tmp2 /= 10;
        if (tmp3 == 1) tab[i] = 1;
        else tab[i] = 0;
    }
}

/******************************************************/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Zla liczba argumentow! Podaj liczbe potomkow i liczbe prosb\n");
        exit(EXIT_FAILURE);
    }
    N = atoi(argv[1]);
    K = atoi(argv[2]);
    pendingProcesses = (int*)calloc(N, sizeof(int));
    int *processesPids = (int*)calloc(N, sizeof(int));

    mainPid = getpid();
    mask = (int*)calloc(5, sizeof(int));
    setMask(mask);
    printf("PPID: %i\n", mainPid);

    struct sigaction sigusr1;
    sigusr1.sa_sigaction = sigUsr1;
    sigusr1.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr1.sa_mask);
    if(sigaction(SIGUSR1, &sigusr1, NULL) < 0)
    {
        printf("Blad podczas rejestrowania obslugi sygnalu SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    struct sigaction sigrt;
    sigrt.sa_sigaction = sigRt;
    sigrt.sa_flags = SA_SIGINFO;
    sigemptyset(&sigrt.sa_mask);
    for(int i = SIGRTMIN ; i <= SIGRTMAX ; i++)
    {
        if(sigaction(i, &sigrt, NULL) < 0)
        {
            printf("Blad podczas rejestrowania obslugi sygnalu SIGRT %i", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0 ; i < N ; i++)
    {
        int pid = fork();
        if(pid == 0)
        {
            sleep(rand() % 10);
            kill(getppid(), SIGUSR1);
            sleep(maxSleep);
            if(chEnabled == 1)
            {
                int seed;
                time_t tt;
                seed = time(&tt);
                srand(seed + getpid());
                kill(getppid(), SIGRTMIN + (rand () % (SIGRTMAX - SIGRTMIN)));
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Proces potomny PID: %i nie otrzymal pozwolenia\n", getpid());
                exit(EXIT_FAILURE);
            }
        }
        else if(pid > 0)
        {
            if(mask[0] == 1) printf("Utworzono %i proces potomny PID: %i\n", i + 1, pid);
            processesPids[i] = pid;
        }
        else
        {
            printf("Blad podczas tworzenia procesu potomnego\n");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid;
  	while(pid = waitpid(-1, NULL, 0))
    {
        if (errno == ECHILD)
        {
            break;
        }
    }

    for(int i = 0 ; i < N ; i++)
    {
        int stat;
        waitpid(processesPids[i], &stat, 0);
        stat = WEXITSTATUS(stat);
        if(mask[4] == 1) printf("Proces %i zakonczony, zwrocona wartosc: %i\n", processesPids[i], stat);
    }

    printf("Otrzymanych sygnalow RLT: %i\n", rtSignals);

    return 0;
}
