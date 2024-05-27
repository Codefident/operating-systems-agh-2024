#define _POSIX_C_SOURCE 199309L

#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;
const int cells_amount = grid_width * grid_height;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

// concurrent

pthread_t *threads = NULL;
thread_data_t *thread_data = NULL;

int init_threads(int n, char *src, char *dst)
{
    if (threads != NULL)
        return 0;

    threads = malloc(cells_amount * sizeof(pthread_t));
    thread_data = malloc(cells_amount * sizeof(thread_data_t));

    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        perror("sigaction() error");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < n; i++)
    {
        thread_data[i].src = src;
        thread_data[i].dst = dst;
        thread_data[i].first_cell = i * (cells_amount / n);
        thread_data[i].last_cell = i * (cells_amount / n) + (cells_amount / n) - 1;

        if (i == n - 1)
            thread_data[i].last_cell = cells_amount - 1;

        pthread_create(&threads[i], NULL, update_cells, (void *)&thread_data[i]);
    }
    return 0;
}

void free_all()
{
    free(threads);
    free(thread_data);
}

void handler()
{
}

void *update_cells(void *thread_data)
{
    thread_data_t *data = thread_data;

    while (true)
    {
        for (int k = data->first_cell; k <= data->last_cell; k++)
        {
            int j = k % grid_width;  // col
            int i = k / grid_height; // row

            data->dst[i * grid_width + j] = is_alive(i, j, data->src);
        }

        pause();

        char *tmp;

        tmp = data->dst;
        data->src = data->dst;
        data->dst = tmp;
    }

    return NULL;
}

void update_grid_concurrent(int n, char *src, char *dst)
{
    if (threads == NULL)
    {
        perror("Threads were not initialized");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++)
    {
        pthread_kill(threads[i], SIGUSR1);
    }
}
