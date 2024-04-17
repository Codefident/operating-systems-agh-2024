#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WIDTH 0.0001

double f(double x)
{
    return 4 / (x * x + 1);
}

double calculate_integral(double a, double b, double width)
{
    double result = 0;
    double x;
    for (x = a; x < b; x += width)
    {
        result += f(x) * width;
    }
    return result;
}

int main()
{
    double a, b;

    // read

    int fd = open("fifo", O_RDONLY);

    if (fd == -1)
    {
        perror("open pipe (read) error");
        return EXIT_FAILURE;
    }

    if (read(fd, &a, sizeof(a)) == -1 || read(fd, &b, sizeof(b)) == -1)
    {
        perror("read from pipe error");
        return EXIT_FAILURE;
    }

    close(fd);

    double result = calculate_integral(a, b, WIDTH);

    // write

    fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open pipe (write) error");
        return EXIT_FAILURE;
    }

    if (write(fd, &result, sizeof(result)) == -1)
    {
        perror("write to pipe error");
        return EXIT_FAILURE;
    }

    close(fd);

    return 0;
}
