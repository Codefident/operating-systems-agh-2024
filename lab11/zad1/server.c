#include "chat.h"

Client clients[MAX_CLIENTS];
int client_count = 0;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *address = argv[1];
    int port = atoi(argv[2]);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sockfd, MAX_CLIENTS);

    int epoll_fd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_sockfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev);

    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int n = 0; n < nfds; n++)
        {
            if (events[n].data.fd == server_sockfd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

                if (client_sockfd != -1)
                {
                    recv(client_sockfd, clients[client_count].id, sizeof(clients[client_count].id), 0);
                    clients[client_count].sockfd = client_sockfd;
                    client_count++;

                    ev.events = EPOLLIN;
                    ev.data.fd = client_sockfd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev);
                }
            }
            else
            {
                handle_client_message(events[n].data.fd);
            }
        }
    }

    close(server_sockfd);
    return 0;
}

void remove_client(int sockfd)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].sockfd == sockfd)
        {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
}

void broadcast_message(const char *msg, int sender_sockfd)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].sockfd != sender_sockfd)
        {
            send(clients[i].sockfd, msg, strlen(msg), 0);
        }
    }
}

void send_private_message(const char *msg, const char *receiver_id)
{
    for (int i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i].id, receiver_id) == 0)
        {
            send(clients[i].sockfd, msg, strlen(msg), 0);
            break;
        }
    }
}

void handle_client_message(int client_sockfd)
{
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_sockfd, buffer, BUFFER_SIZE, 0);

    if (bytes_read <= 0)
    {
        remove_client(client_sockfd);
        close(client_sockfd);
        return;
    }

    buffer[bytes_read] = '\0';
    char *command = strtok(buffer, " ");

    if (strcmp(command, "LIST") == 0)
    {
        char list_msg[BUFFER_SIZE] = "Active clients:\n";
        for (int i = 0; i < client_count; i++)
        {
            strcat(list_msg, clients[i].id);
            strcat(list_msg, "\n");
        }
        send(client_sockfd, list_msg, strlen(list_msg), 0);
    }
    else if (strcmp(command, "2ALL") == 0)
    {
        char *msg = strtok(NULL, "\0");
        if (msg)
        {
            char broadcast_msg[BUFFER_SIZE];
            sprintf(broadcast_msg, "From %s: %s", clients[client_sockfd].id, msg);
            broadcast_message(broadcast_msg, client_sockfd);
        }
    }
    else if (strcmp(command, "2ONE") == 0)
    {
        char *receiver_id = strtok(NULL, " ");
        char *msg = strtok(NULL, "\0");
        if (receiver_id && msg)
        {
            char private_msg[BUFFER_SIZE];
            sprintf(private_msg, "From %s: %s", clients[client_sockfd].id, msg);
            send_private_message(private_msg, receiver_id);
        }
    }
    else if (strcmp(command, "STOP") == 0)
    {
        remove_client(client_sockfd);
        close(client_sockfd);
    }
}
