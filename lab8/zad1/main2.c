#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <wait.h>

#define NUM_USERS 3
#define NUM_PRINTERS 2
#define PRINT_QUEUE_SIZE 5
#define TEXT_LENGTH 10
#define EMPTY -1

typedef struct
{
    char text[TEXT_LENGTH];
    int printer_id;
} PrintTask;

void user_process(int user_id, int sem_id, PrintTask *print_queue)
{
    srand(time(NULL));
    while (1)
    {
        // random text
        char text[TEXT_LENGTH];
        for (int i = 0; i < TEXT_LENGTH; i++)
        {
            text[i] = 'a' + rand() % 26;
        }
        // text[TEXT_LENGTH] = '\0';

        // wait
        int wait_time = rand() % 1 + 5;
        sleep(wait_time);

        // Try to add print task to queue
        struct sembuf sem_op;
        sem_op.sem_num = 0;
        sem_op.sem_op = -1; // decrement
        sem_op.sem_flg = 0;
        semop(sem_id, &sem_op, 1);

        int queue_index = 0;
        while (queue_index < PRINT_QUEUE_SIZE && print_queue[queue_index].printer_id != -1)
        {
            queue_index++;
        }

        if (queue_index < PRINT_QUEUE_SIZE)
        {
            // Add print task to queue
            strcpy(print_queue[queue_index].text, text);
            print_queue[queue_index].printer_id = -1; // Mark as not printed yet

            printf("User %d: Submitted print task - %s\n", user_id, text);
        }
        else
        {
            printf("User %d: Print queue full, unable to submit print task - %s\n", user_id, text);
        }

        sem_op.sem_op = 1; // Increment semaphore value
        semop(sem_id, &sem_op, 1);
    }
}

void printer_process(int printer_id, int sem_id, PrintTask *print_queue)
{
    while (1)
    {
        // check if any task is available
        struct sembuf sem_op;
        sem_op.sem_num = 0;
        sem_op.sem_op = -1; // decrement
        sem_op.sem_flg = 0;
        semop(sem_id, &sem_op, 1);

        // look for task
        int queue_index = 0;
        while (queue_index < PRINT_QUEUE_SIZE && print_queue[queue_index].printer_id == -1)
        {
            queue_index++;
        }

        if (queue_index < PRINT_QUEUE_SIZE)
        {
            // print
            PrintTask task = print_queue[queue_index];
            print_queue[queue_index].printer_id = printer_id;

            printf("Printer %d: Printing task from user %d - %s\n", printer_id, task.printer_id, task.text);
            for (int i = 0; i < TEXT_LENGTH; i++)
            {
                putchar(task.text[i]);
                fflush(stdout);
                sleep(1);
            }
            printf("\n");
        }

        sem_op.sem_op = 1; // Increment semaphore value
        semop(sem_id, &sem_op, 1);
    }
}

int main()
{
    srand(time(NULL));

    int sem_id, shm_id;

    // create semaphore
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // initialize
    semctl(sem_id, 0, SETVAL, 1);

    // create shared memory for print queue
    shm_id = shmget(IPC_PRIVATE, PRINT_QUEUE_SIZE * sizeof(PrintTask), IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // get addr of shared memory
    PrintTask *print_queue = (PrintTask *)shmat(shm_id, NULL, 0);

    for (int i = 0; i < PRINT_QUEUE_SIZE; i++)
    {
        print_queue[i].printer_id = EMPTY;
    }

    // Fork user processes
    for (int i = 0; i < NUM_USERS; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            user_process(i + 1, sem_id, print_queue);
            exit(EXIT_SUCCESS);
        }
        else if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Fork printer processes
    for (int i = 0; i < NUM_PRINTERS; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            printer_process(i + 1, sem_id, print_queue);
            exit(EXIT_SUCCESS);
        }
        else if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    wait(NULL);

    shmdt(print_queue);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);

    return EXIT_SUCCESS;
}
