#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <time.h>

#define BUFFER_SIZE 3
#define TEXT_LENGTH 10

typedef struct
{
    char to_print[BUFFER_SIZE][TEXT_LENGTH + 1];
    int in;
    int out;
} print_buffer_t;

#define SHM_KEY 1234
#define SEM_KEY 5678

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

void sem_init(int sem_id);
void sem_wait(int sem_id, int sem_num);
void sem_signal(int sem_id, int sem_num);
int sem_getval(int sem_id, int sem_num);
