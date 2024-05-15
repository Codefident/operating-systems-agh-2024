#include "print_system.h"

int main()
{
    srand(time(NULL) + getpid());

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
        char text[TEXT_LENGTH + 1];
        strcpy(text, "");

        for (int i = 0; i < TEXT_LENGTH; i++)
        {
            text[i] = 'a' + rand() % 26;
        }
        text[TEXT_LENGTH] = '\0';

        printf("Text to print: %s\n", text);

        wait_semaphore(sem_id, SEM_EMPTY);
        wait_semaphore(sem_id, SEM_MUTEX);

        // memset(buffer->to_print[buffer->in], 0, sizeof(buffer->to_print[buffer->in]));
        strcpy(buffer->to_print[buffer->in], text);
        buffer->in = (buffer->in + 1) % BUFFER_SIZE;

        printf("Text is now in printing queue (buffer)\n\n");

        // for (int i = 0; i < BUFFER_SIZE; i++)
        // {
        //     printf("buffer[%d]: %s\n", i, buffer->to_print[i]);
        // }

        signal_semaphore(sem_id, SEM_MUTEX);
        signal_semaphore(sem_id, SEM_FULL);

        sleep(rand() % 5 + 1);
    }

    return EXIT_SUCCESS;
}
