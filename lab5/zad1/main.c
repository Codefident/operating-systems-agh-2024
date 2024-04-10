#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

void handler(int signum);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Wywolanie: %s <[none, ignore, handler, mask]>\n", argv[0]);
        return EXIT_FAILURE;
    }

    sigset_t base_mask, waiting_mask;
    sigemptyset(&base_mask);
    sigaddset(&base_mask, SIGUSR1);

    if (strcmp(argv[1], "none") == 0)
    {
        //// nic nie robimy
    }

    else if (strcmp(argv[1], "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
    }

    else if (strcmp(argv[1], "handler") == 0)
    {
        signal(SIGUSR1, &handler);
    }

    else if (strcmp(argv[1], "mask") == 0)
    {
        if (sigprocmask(SIG_BLOCK, &base_mask, NULL) < 0)
        {
            perror("signals not blocked");
        }
    }

    else
    {
        fprintf(stderr, "Nieznana opcja\n");
        return EXIT_FAILURE;
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "mask") != 0)
    {
        return 0;
    }

    sigpending(&waiting_mask);
    if (sigismember(&waiting_mask, SIGUSR1))
    {
        printf("SIGUSR1 jest widoczny\n");
    }
    else
    {
        printf("SIGUSR1 nie jest widoczny\n");
    }

    return 0;
}

void handler(int signum)
{
    printf("Otrzymano sygnal %d\n", signum);
}
