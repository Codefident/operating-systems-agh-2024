#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const int BLOCK_SIZE = 1024;

void reverseSingle(FILE *rfile, FILE *wfile);
void reverseBlock(FILE *rfile, FILE *wfile);

int main(int argc, char *argv[])
{
    clock_t start, end;

    start = clock();

    if (argc != 4)
    {
        printf("Usage: %s <s for single | b for block> <input_file> <output_file>\n", argv[0]);
        perror("Wrong number of arguments\n");
        return 1;
    }

    FILE *rfile = fopen(argv[2], "rb");
    FILE *wfile = fopen(argv[3], "wb");

    if (rfile == NULL || wfile == NULL)
    {
        perror("File was not opened");
        return 1;
    }

    if (strcmp(argv[1], "s") == 0)
        reverseSingle(rfile, wfile);
    else if (strcmp(argv[1], "b") == 0)
        reverseBlock(rfile, wfile);
    else
        printf("File not reversed\n%s - unknown argument, use s or b\n", argv[1]);

    fclose(rfile);
    fclose(wfile);

    end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    // double time = (end - start) / CLOCKS_PER_SEC;

    printf("Time: %f s\n", time);

    return 0;
}

void reverseSingle(FILE *rfile, FILE *wfile)
{
    char buffer;
    int offset = 0;
    while (fseek(rfile, --offset, SEEK_END) == 0)
    {
        fread(&buffer, sizeof(char), 1, rfile);
        fwrite(&buffer, sizeof(char), 1, wfile);
    }
}

void reverseBlock(FILE *rfile, FILE *wfile)
{
    char buffer[BLOCK_SIZE];

    fseek(rfile, 0, SEEK_END);
    int charsRemaining = (int)ftell(rfile);

    // read whole blocks
    while (charsRemaining >= BLOCK_SIZE)
    {
        fseek(rfile, -BLOCK_SIZE, SEEK_CUR);
        fread(buffer, sizeof(char), BLOCK_SIZE, rfile);

        char *reversed = malloc(sizeof(char) * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE; i++)
            reversed[i] = buffer[BLOCK_SIZE - 1 - i];

        fwrite(reversed, sizeof(char), BLOCK_SIZE, wfile);
        free(reversed);

        charsRemaining -= BLOCK_SIZE;
        fseek(rfile, -BLOCK_SIZE, SEEK_CUR);
    }

    // read remaining characters if there are any
    if (charsRemaining > 0)
    {
        fseek(rfile, 0, SEEK_SET);
        fread(buffer, sizeof(char), charsRemaining, rfile);

        char *reversed = malloc(sizeof(char) * charsRemaining);

        for (int i = 0; i < charsRemaining; i++)
            reversed[i] = buffer[charsRemaining - 1 - i];

        fwrite(reversed, sizeof(char), charsRemaining, wfile);
        free(reversed);
    }
}