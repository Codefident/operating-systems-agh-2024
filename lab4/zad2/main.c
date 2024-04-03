#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

unsigned int global;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No arguments passed\n");
        return 0;
    }

    unsigned int local;

    global = 0;
    local = 0;

    printf("%s\n", argv[0]);

    pid_t pid = fork();

    // error
    if (pid == -1)
    {
        perror("fork() error");
        return 1;
    }

    // proces dziecka
    if (pid == 0)
    {
        printf("child process\n");

        global++;
        local++;

        pid_t child_pid = getpid();
        pid_t parent_pid = getppid();

        printf("child pid = %d, parent pid = %d\n", child_pid, parent_pid);
        printf("child's local = %d, child's global = %d\n", local, global);

        // NULL - koniec listy argumentow
        int error = execl("/bin/ls", "ls", argv[1], NULL);
        perror("execl() error");
        exit(error);
    }

    // proces rodzica
    int status;
    pid_t child_pid = waitpid(pid, &status, 0);

    if (child_pid == -1)
    {
        perror("waitpid() error");
        return 1;
    }

    printf("parent process\n");

    pid_t parent_pid = getpid();
    printf("parent pid = %d, child pid = %d\n", parent_pid, child_pid);

    int exit_status;
    if (WIFEXITED(status))
    {
        exit_status = WEXITSTATUS(status);
        printf("child exit code: %d\n", exit_status);
    }

    printf("parent's local = %d, parent's global = %d\n", local, global);

    return exit_status;
}