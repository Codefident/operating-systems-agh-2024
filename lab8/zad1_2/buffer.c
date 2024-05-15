#include "print_system.h"

int main()
{
    int shm_id = shmget(SHM_KEY, sizeof(print_buffer_t), IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    print_buffer_t *buffer = (print_buffer_t *)shmat(shm_id, NULL, 0);
    if (buffer == (void *)-1)
    {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    buffer->in = 0;
    buffer->out = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        strcpy(buffer->to_print[i], "");
    }

    int sem_id = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if (sem_id == -1)
    {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    init_semaphores(sem_id);

    while (1)
    {
        sleep(1);
    }

    return EXIT_SUCCESS;
}
