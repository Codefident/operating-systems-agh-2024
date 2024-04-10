#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

volatile sig_atomic_t command;
volatile sig_atomic_t commands_received = 0;
volatile sig_atomic_t perform_action = 0;

void handler(int signum, siginfo_t *info, void *context)
{
    if (signum == SIGUSR1)
    {
        command = info->si_value.sival_int;
        printf("Otrzymano SIGUSR1, command: %d\n", command);

        commands_received++;
        perform_action = 1;

        kill(info->si_pid, SIGUSR1);
    }
}

int main()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        fprintf(stderr, "sigaction() error\n");
        return EXIT_FAILURE;
    }

    printf("Catcher PID: %d\n", getpid());
    printf("Oczekiwanie na sygnaly...\n");

    while (command != 3)
    {
        while (perform_action == 0)
        {
            pause();
        }

        perform_action = 0;

        switch (command)
        {
        case 1:
            for (int i = 1; i <= 100; i++)
            {
                printf("%d\n", i);
            }
            break;
        case 2:
            printf("Otrzymane polecenia od sendera: %d\n", commands_received);
            break;

        case 3:
            printf("Koncze dzialanie catchera\n");
            return 0;

        default:
            printf("Nie znam takiego polecenia\n");
            break;
        }
    }

    return 0;
}
