#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);

// concurrent
typedef struct
{
    char *src;
    char *dst;
    int first_cell;
    int last_cell;
} thread_data_t;

int init_threads(int n, char *src, char *dst);
void free_all();
void handler();
void *update_cells(void *thread_data);
void update_grid_concurrent(int n, char *src, char *dst);
