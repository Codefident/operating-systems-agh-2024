#ifndef PRINT_SYSTEM_H
#define PRINT_SYSTEM_H

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
#define JOB_LENGTH 10

typedef struct
{
    char jobs[BUFFER_SIZE][JOB_LENGTH];
    int job_count;
    int in;
    int out;
} print_buffer_t;

#define SHM_KEY 1234
#define SEM_KEY 5678

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

void init_semaphores(int sem_id);
void wait_semaphore(int sem_id, int sem_num);
void signal_semaphore(int sem_id, int sem_num);

#endif // PRINT_SYSTEM_H
