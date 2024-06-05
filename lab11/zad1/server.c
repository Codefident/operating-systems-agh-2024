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

    // program params
    const char *address = argv[1];
    int port = atoi(argv[2]);

    // create socket
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // === INITIALIZE SERVER ===

    // declare server params
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    // bind server params to server socket descriptor
    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sockfd, MAX_CLIENTS);

    // === EPOLL ===

    // create epoll
    int epoll_fd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_sockfd;

    // register server socket descriptor to epoll
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev);

    while (1)
    {
        int nevnts = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int n = 0; n < nevnts; n++)
        {
            // add new client
            if (events[n].data.fd == server_sockfd)
            {
                // client params
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

                if (client_sockfd != -1)
                {
                    recv(client_sockfd, clients[client_count].id, sizeof(clients[client_count].id), 0);
                    printf("Adding new user: %s\n", clients[client_count].id);
                    clients[client_count].sockfd = client_sockfd;
                    client_count++;

                    ev.events = EPOLLIN;
                    ev.data.fd = client_sockfd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev);
                }
            }

            // process client's message
            else
            {
                handle_client_message(events[n].data.fd);
            }
        }
    }

    close(server_sockfd);
    return 0;
}

void get_current_time(char *buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", t);
}

int get_client_index(int client_sockfd)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].sockfd == client_sockfd)
        {
            return i;
        }
    }
    return 0;
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

    // === commands ===

    // LIST
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

    // 2ALL
    else if (strcmp(command, "2ALL") == 0)
    {
        char *msg = strtok(NULL, "\0");
        if (msg)
        {
            char broadcast_msg[BUFFER_SIZE];
            char current_time[64];
            get_current_time(current_time, sizeof(current_time));
            int client_index = get_client_index(client_sockfd);

            sprintf(broadcast_msg, "%s (%s): %s", clients[client_index].id, current_time, msg);
            broadcast_message(broadcast_msg, client_sockfd);
        }
    }

    // 2ONE
    else if (strcmp(command, "2ONE") == 0)
    {
        char *receiver_id = strtok(NULL, " ");
        char *msg = strtok(NULL, "\0");
        if (receiver_id && msg)
        {
            char private_msg[BUFFER_SIZE];
            char current_time[64];
            get_current_time(current_time, sizeof(current_time));
            int client_index = get_client_index(client_sockfd);

            sprintf(private_msg, "%s (%s): %s", clients[client_index].id, current_time, msg);
            send_private_message(private_msg, receiver_id);
        }
    }

    // STOP
    else if (strcmp(command, "STOP") == 0)
    {
        remove_client(client_sockfd);
        close(client_sockfd);
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

void remove_client(int sockfd)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].sockfd == sockfd)
        {
            printf("Removing client: %s\n", clients[i].id);
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
}
