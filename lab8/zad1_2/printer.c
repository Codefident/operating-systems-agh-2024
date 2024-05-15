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

        char job[JOB_LENGTH + 1];
        strcpy(job, buffer->jobs[buffer->out]);
        buffer->out = (buffer->out + 1) % BUFFER_SIZE;
        buffer->job_count--;

        signal_semaphore(sem_id, SEM_MUTEX);
        signal_semaphore(sem_id, SEM_EMPTY);

        printf("Printing: %s\n", job);

        for (int i = 0; i < JOB_LENGTH; ++i)
        {
            printf("%c", job[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
