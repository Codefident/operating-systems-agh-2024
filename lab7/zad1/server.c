#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_CLIENTS 10
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
    int server_queue_id;
    int client_queue_ids[MAX_CLIENTS];
    int num_clients = 0;

    // create server queue
    if ((server_queue_id = msgget(SERVER_KEY, IPC_CREAT | 0666)) == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("Server started...\n");

    // receiving messages and dealing with them
    while (1)
    {
        struct client_message message;

        if (msgrcv(server_queue_id, &message, sizeof(message) - sizeof(long), 0, 0) == -1)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        switch (message.mtype)
        {
        case INIT: // initialize message

            // create a client's queue
            key_t client_key = atoi(message.mtext);
            int client_queue_id;

            if ((client_queue_id = msgget(client_key, 0666)) == -1)
            {
                perror("msgget");
                exit(EXIT_FAILURE);
            }

            message.client_id = num_clients + 1;
            num_clients++;
            printf("message.client_id = %d\n", message.client_id);

            // send back id
            message.mtype = message.client_id;

            if (msgsnd(client_queue_id, &message, sizeof(message) - sizeof(long), 0) == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }

            client_queue_ids[num_clients - 1] = client_queue_id;

            printf("Client %d connected.\n", message.client_id);
            break;

        case MSG: // regular message from client, send it to everyone else

            for (int i = 0; i < num_clients; i++)
            {
                if (i != message.client_id - 1)
                {
                    if (msgsnd(client_queue_ids[i], &message, sizeof(message) - sizeof(long), 0) == -1)
                    {
                        perror("msgsnd");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            break;

        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}
