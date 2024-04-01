#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No arguments passed\n");
        return 0;
    }

    // Konwersja ze stringa do inta
    int n = atoi(argv[1]);

    if (n == 0 && argv[1][0] != '0')
    {
        fprintf(stderr, "Error: Invalid argument, not a number\n");
        return 1;
    }

    pid_t children_pids[n];

    // Tworzenie n procesow
    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();

        // error
        if (pid == -1)
        {
            perror("fork() error");
            return 1;
        }

        children_pids[i] = pid;

        // nowy proces
        if (pid == 0)
        {
            pid_t ppid = getppid();
            pid_t pid_child = getpid();
            printf("%d, %d\n", ppid, pid_child);
            exit(0);
        }
    }

    // czekanie na potomkow
    for (int i = 0; i < n; i++)
        waitpid(children_pids[i], NULL, 0);

    printf("%s\n", argv[1]);

    return 0;
}