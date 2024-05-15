#include "print_system.h"

void sem_init(int sem_id)
{
    semctl(sem_id, SEM_MUTEX, SETVAL, 1);
    semctl(sem_id, SEM_EMPTY, SETVAL, BUFFER_SIZE);
    semctl(sem_id, SEM_FULL, SETVAL, 0);
}

void sem_wait(int sem_id, int sem_num)
{
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void sem_signal(int sem_id, int sem_num)
{
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

int sem_getval(int sem_id, int sem_num)
{
    int value = semctl(sem_id, sem_num, GETVAL);
    if (value == -1)
    {
        perror("semctl GETVAL failed");
        exit(EXIT_FAILURE);
    }
    return value;
}
