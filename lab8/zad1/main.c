#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>

#define N_USERS 3
#define M_PRINTERS 2
#define QUEUE_SIZE 4
#define QUEUE_NUM 2
#define TEXT_LENGTH 10

// status
#define EMPTY 0
#define WAITING 1
#define PRINTING 2

typedef struct
{
    char text[TEXT_LENGTH];
    int status;
    int user_id;
} PrintTask;

// int sem_id_users;
// int sem_id_printers;
// int sem_id_queue;

int create_semaphore(int, int);
void printer_process(int, int, PrintTask *);
void user_process(int, int, PrintTask *);

int main()
{
    key_t key;
    int shm_id;

    // semaphores
    // sem_id_users = create_semaphore(0, N_USERS);
    // sem_id_printers = create_semaphore(1, M_PRINTERS);
    int sem_id_queue = create_semaphore(QUEUE_NUM, QUEUE_SIZE);
    // int sem_id_printers = create_semaphore(1, M_PRINTERS);

    // shared memory for queue
    key = ftok(".", QUEUE_NUM);
    if (key == -1)
    {
        perror("ftok, shared memory");
        exit(EXIT_FAILURE);
    }

    shm_id = shmget(key, QUEUE_SIZE * sizeof(PrintTask), IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    PrintTask *print_queue = (PrintTask *)shmat(shm_id, NULL, 0);

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        PrintTask task;
        task.status = EMPTY;
        task.user_id = -1;
        print_queue[i] = task;
    }

    // create users
    for (int i = 0; i < N_USERS; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("user fork");
            exit(EXIT_FAILURE);
        }

        else if (pid == 0)
        {
            user_process(i + 1, sem_id_queue, print_queue);
            exit(EXIT_SUCCESS);
        }
    }

    // create printers
    for (int i = 0; i < M_PRINTERS; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("printer fork");
            exit(EXIT_FAILURE);
        }

        else if (pid == 0)
        {
            printer_process(i + 1, sem_id_queue, print_queue);
            exit(EXIT_SUCCESS);
        }
    }

    wait(NULL);
    printf("Finishing");

    shmdt(print_queue);
    shmctl(shm_id, IPC_RMID, NULL);
    // semctl(sem_id_users, 0, IPC_RMID);
    // semctl(sem_id_printers, 0, IPC_RMID);
    semctl(sem_id_queue, 0, IPC_RMID);

    return EXIT_SUCCESS;
}

int create_semaphore(int num, int initial_value)
{
    key_t key;
    int sem_id;

    key = ftok(".", num);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (sem_id == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(sem_id, 0, SETVAL, initial_value) == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return sem_id;
}

void user_process(int user_id, int sem_id, PrintTask *print_queue)
{
    printf("User %d here!\n", user_id);

    srand(time(NULL) + getpid());
    int to_print = 2;

    while (to_print > 0)
    {
        // random text
        char text[TEXT_LENGTH];
        for (int i = 0; i < TEXT_LENGTH; i++)
        {
            text[i] = 'a' + rand() % 26;
        }

        // wait
        int wait_time = rand() % 20 + 3;
        sleep(wait_time);

        // try to add to queue
        struct sembuf sem_op;
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        semop(sem_id, &sem_op, 1);

        int queue_index = 0;
        while (queue_index < QUEUE_SIZE && print_queue[queue_index].status != EMPTY)
        {
            queue_index++;
        }

        if (queue_index < QUEUE_SIZE)
        {
            // Add print task to queue
            strcpy(print_queue[queue_index].text, text);
            print_queue[queue_index].status = WAITING;
            print_queue[queue_index].user_id = user_id;

            printf("User %d: Submitted print task - %s\n", user_id, text);
            to_print--;
        }
        else
        {
            printf("User %d: Print queue full, unable to submit print task - %s\n", user_id, text);
            to_print--;
        }
    }
    exit(EXIT_SUCCESS);
}

void printer_process(int printer_id, int sem_id, PrintTask *print_queue)
{
    printf("Printer %d here!\n", printer_id);

    while (1)
    {
        // check if any task is available

        // look for task
        int queue_index = 0;
        while (queue_index < QUEUE_SIZE && print_queue[queue_index].status != WAITING)
        {
            queue_index++;
        }

        if (queue_index < QUEUE_SIZE)
        {
            // print
            PrintTask task = print_queue[queue_index];
            print_queue[queue_index].status = PRINTING;

            printf("Printer %d: Printing task from user %d - %s\n", printer_id, task.user_id, task.text);
            for (int i = 0; i < TEXT_LENGTH; i++)
            {
                // putchar(task.text[i]);
                // fflush(stdout);
                printf("%c\n", task.text[i]);
                sleep(1);
            }
            printf("\n");

            print_queue[queue_index].status = EMPTY;
        }

        struct sembuf sem_op;
        sem_op.sem_num = 0;
        sem_op.sem_op = 1; // increment
        sem_op.sem_flg = 0;
        semop(sem_id, &sem_op, 1);
    }
    exit(EXIT_SUCCESS);
}
