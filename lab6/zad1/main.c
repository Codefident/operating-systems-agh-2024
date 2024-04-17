#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

double f(double x)
{
    return 4 / (x * x + 1);
}

double calculate_integral(double start, double end, double width)
{
    double result = 0;
    double x;
    for (x = start; x < end; x += width)
    {
        result += f(x) * width;
    }
    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uzycie programu: %s <width> <n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    double width = atof(argv[1]);
    int n = atoi(argv[2]);
    double total_result = 0;

    // time
    clock_t time_start, time_end;
    time_start = clock();

    // pipes
    int i;
    int pipes[n][2];

    // child processes
    for (i = 0; i < n; i++)
    {
        pid_t pid;

        if (pipe(pipes[i]) == -1)
        {
            perror("pipe error");
            return EXIT_FAILURE;
        }

        pid = fork();

        if (pid < 0)
        {
            perror("fork error\n");
            return EXIT_FAILURE;
        }

        // child process
        if (pid == 0)
        {
            close(pipes[i][0]); // Close reading

            double start = (double)i / n;
            double end = (double)(i + 1) / n;
            double result = calculate_integral(start, end, width);

            write(pipes[i][1], &result, sizeof(result));
            close(pipes[i][1]);
            exit(0);
        }
    }

    // sum up results
    for (i = 0; i < n; i++)
    {
        double result;
        if (read(pipes[i][0], &result, sizeof(result)) != -1)
            total_result += result;

        close(pipes[i][0]);
    }

    // wait for all the n child processes
    for (i = 0; i < n; i++)
    {
        wait(NULL);
    }

    // end time
    time_end = clock();
    double time = (double)(time_end - time_start) / CLOCKS_PER_SEC;

    printf("Wynik: %f\n", total_result);
    printf("Czas: %f s\n", time);

    return 0;
}
