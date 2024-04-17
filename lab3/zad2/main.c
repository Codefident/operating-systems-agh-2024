#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main()
{
    DIR *dir = opendir("./");

    if (dir == NULL)
    {
        perror("Error when opening directory");
        return 1;
    }

    struct dirent *file;
    struct stat fileStat;
    unsigned long long totalSize = 0;

    while ((file = readdir(dir)) != NULL)
    {
        if (stat(file->d_name, &fileStat) == -1)
        {
            perror("Error when getting the stats of a file");
            continue;
        }

        if (S_ISDIR(fileStat.st_mode))
        {
            // printf("%s is a directory\n", file->d_name);
            continue;
        }

        // if not... //
        printf("\nFile name: %s\nFile size: %ld bytes\n", file->d_name, fileStat.st_size);
        totalSize += fileStat.st_size;
    }

    printf("\n============\n\nTotal size of all files: %lld bytes\n\n", totalSize);
    closedir(dir);

    return 0;
}