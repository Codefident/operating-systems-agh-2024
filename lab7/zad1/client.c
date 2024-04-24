#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_KEY 0x123123ab

#define INIT 1
#define MSG 3

struct client_message
{
    long mtype;
    int client_id;
    char mtext[256];
};

int main()
{
    key_t client_key;
    int client_queue_id;
    int server_queue_id;
    int client_id = -1;

    // generate random client key
    if ((client_key = ftok(".", getpid())) == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // create client queue
    if ((client_queue_id = msgget(client_key, IPC_CREAT | 0666)) == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // send init
    struct client_message init_message;
    init_message.mtype = INIT;
    sprintf(init_message.mtext, "%d", client_key);

    if ((server_queue_id = msgget(SERVER_KEY, 0666)) == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    if (msgsnd(server_queue_id, &init_message, sizeof(init_message) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server...\n");

    // get id from server
    struct client_message response;
    if (msgrcv(client_queue_id, &response, sizeof(response) - sizeof(long), 0, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Received client ID: %d\n", response.client_id);
    client_id = response.client_id;

    // new process to receive messages
    if (fork() == 0)
    {
        while (1)
        {
            struct client_message message;

            if (msgrcv(client_queue_id, &message, sizeof(message) - sizeof(long), 0, 0) == -1)
            {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }

            printf("Message from %d: %s\n", message.client_id, message.mtext);
        }
    }

    // send message to server
    while (1)
    {
        struct client_message message;

        message.mtype = MSG;
        message.client_id = client_id;
        fgets(message.mtext, sizeof(message.mtext), stdin);

        if (msgsnd(server_queue_id, &message, sizeof(message) - sizeof(long), 0) == -1)
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
