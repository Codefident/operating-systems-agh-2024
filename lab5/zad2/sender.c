#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int signum);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Wywolanie: %s <catcher_PID> <[1, 2, 3]>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        perror("sigaction() error");
        return EXIT_FAILURE;
    }

    printf("Sender PID: %d\n", getpid());

    union sigval value;
    value.sival_int = mode;
    if (sigqueue(catcher_pid, SIGUSR1, value) == -1)
    {
        perror("sigqueue() error");
        return EXIT_FAILURE;
    }

    printf("Wyslano sygnal do catchera\n");

    pause();

    return 0;
}

void handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Otrzymano SIGUSR1 od catchera\n");
    }
}