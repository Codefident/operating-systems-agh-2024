#include "print_system.h"

void init_semaphores(int sem_id)
{
    semctl(sem_id, SEM_MUTEX, SETVAL, 1);
    semctl(sem_id, SEM_EMPTY, SETVAL, BUFFER_SIZE);
    semctl(sem_id, SEM_FULL, SETVAL, 0);
}

void wait_semaphore(int sem_id, int sem_num)
{
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void signal_semaphore(int sem_id, int sem_num)
{
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}
