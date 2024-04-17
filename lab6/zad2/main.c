#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    double a, b, result;

    printf("Podaj poczatek przedzialu: ");
    scanf("%lf", &a);
    printf("Podaj koniec przedzialu: ");
    scanf("%lf", &b);

    // create named pipe, 6(rw) - r(4) + w(2)
    // 0 - no special
    if (mkfifo("fifo", 0666) == -1)
    {
        perror("mkfifo error");
        return EXIT_FAILURE;
    }

    // write

    int fd = open("fifo", O_WRONLY);

    if (fd == -1)
    {
        perror("open pipe (write) error");
        return EXIT_FAILURE;
    }

    if (write(fd, &a, sizeof(a)) == -1 || write(fd, &b, sizeof(b)) == -1)
    {
        perror("write to pipe error");
        return EXIT_FAILURE;
    }

    close(fd);

    // read

    fd = open("fifo", O_RDONLY);

    if (fd == -1)
    {
        perror("open pipe error");
        return EXIT_FAILURE;
    }

    if (read(fd, &result, sizeof(result)) == -1)
    {
        perror("read from pipe error");
        return EXIT_FAILURE;
    }

    close(fd);

    printf("Wynik: %lf\n", result);

    return 0;
}
