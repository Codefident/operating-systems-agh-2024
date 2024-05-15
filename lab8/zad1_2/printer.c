#include "print_system.h"

int main()
{
    int shm_id = shmget(SHM_KEY, sizeof(print_buffer_t), 0666);
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

    int sem_id = semget(SEM_KEY, 3, 0666);
    if (sem_id == -1)
    {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        wait_semaphore(sem_id, SEM_FULL);
        wait_semaphore(sem_id, SEM_MUTEX);

        char text[TEXT_LENGTH + 1];
        strcpy(text, buffer->to_print[buffer->out]);
        buffer->out = (buffer->out + 1) % BUFFER_SIZE;

        signal_semaphore(sem_id, SEM_MUTEX);
        signal_semaphore(sem_id, SEM_EMPTY);

        printf("Printing: %s\n", text);

        for (int i = 0; i < TEXT_LENGTH; i++)
        {
            printf("%c", text[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n\n");
    }

    return EXIT_SUCCESS;
}
