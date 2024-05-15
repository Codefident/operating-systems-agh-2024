#include "print_system.h"

void generate_random_job(char *job)
{
    for (int i = 0; i < JOB_LENGTH; ++i)
    {
        job[i] = 'a' + rand() % 26;
    }
    job[JOB_LENGTH] = '\0';
}

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
        char job[JOB_LENGTH + 1];
        generate_random_job(job);

        printf("Text to print: %s\n", job);

        wait_semaphore(sem_id, SEM_EMPTY);
        wait_semaphore(sem_id, SEM_MUTEX);

        strcpy(buffer->jobs[buffer->in], job);
        buffer->in = (buffer->in + 1) % BUFFER_SIZE;
        buffer->job_count++;

        signal_semaphore(sem_id, SEM_MUTEX);
        signal_semaphore(sem_id, SEM_FULL);

        sleep(rand() % 5 + 1);
    }

    return EXIT_SUCCESS;
}
