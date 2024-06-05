#include "chat.h"

int sockfd;
char client_id[20];

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Use it like: %s <client_id> <ip_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // program params
    strcpy(client_id, argv[1]);
    const char *address = argv[2];
    int port = atoi(argv[3]);

    // ctrl + c handler
    signal(SIGINT, handle_signal);

    chat_client(address, port);

    return EXIT_SUCCESS;
}

void chat_client(const char *address, int port)
{
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // initial message, register user on server
    send(sockfd, client_id, strlen(client_id), 0);

    fd_set readfds;
    char buffer[1024];

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // from socket and stdin
        int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            fgets(buffer, 1024, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strncmp(buffer, "STOP", 4) == 0)
            {
                cleanup();
                exit(EXIT_SUCCESS);
            }

            send(sockfd, buffer, strlen(buffer), 0);
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            int bytes_received = recv(sockfd, buffer, 1024, 0);
            if (bytes_received > 0)
            {
                buffer[bytes_received] = '\0';
                printf("%s\n", buffer);
            }
        }
    }
}

void cleanup()
{
    send(sockfd, "STOP", 4, 0);
    close(sockfd);
}

void handle_signal(int sig)
{
    cleanup();
    exit(EXIT_SUCCESS);
}
