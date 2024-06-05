#include "chat.h"

Client clients[MAX_CLIENTS];
int client_count = 0;

int server_sockfd;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Use it like: %s <ip_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // program params
    const char *address = argv[1];
    int port = atoi(argv[2]);

    // create socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // === INITIALIZE SERVER ===

    // declare server params
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address); // XXX.XXX.XXX to binary
    server_addr.sin_port = htons(port);               // 16-bit number!

    // bind server params to server socket descriptor
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("server bind error");
        return EXIT_FAILURE;
    }

    // === EPOLL ===

    // create epoll
    int epoll_fd = epoll_create1(0);

    if (epoll_fd == -1)
    {
        perror("server epoll_create1 error");
        return EXIT_FAILURE;
    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_sockfd;

    // register server socket descriptor to epoll
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev) == -1)
    {
        perror("server epoll_ctl error");
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1)
    {
        int nevnts = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int n = 0; n < nevnts; n++)
        {
            if (events[n].data.fd == server_sockfd)
            {
                int bytes_read = recvfrom(server_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
                buffer[bytes_read] = '\0';

                // check if client already exists
                int client_idx = -1;
                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (
                        clients[i].sockaddr.sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                        clients[i].sockaddr.sin_port == client_addr.sin_port)
                    {
                        client_idx = i;
                        break;
                    }
                }

                // user doesnt exist, add client
                if (client_idx == -1)
                {
                    printf("Adding new client: %s\n", buffer);
                    clients[client_count].sockaddr = client_addr;
                    strncpy(clients[client_count].id, buffer, MAX_NAME_LEN);
                    client_idx = client_count;
                    client_count++;
                }

                // process client's message
                else
                {
                    handle_client_message(buffer, bytes_read, client_idx);
                }
            }
        }
    }

    close(server_sockfd);
    return EXIT_SUCCESS;
}

void get_current_time(char *buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", t);
}

void handle_client_message(char *buffer, int bytes_read, int client_idx)
{
    if (bytes_read <= 0)
    {
        remove_client(client_idx);
        return;
    }

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
        sendto(server_sockfd, list_msg, strlen(list_msg), 0, (struct sockaddr *)&clients[client_idx].sockaddr, sizeof(clients[client_idx].sockaddr));
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

            sprintf(broadcast_msg, "%s (%s): %s", clients[client_idx].id, current_time, msg);
            broadcast_message(broadcast_msg, client_idx);
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

            sprintf(private_msg, "%s (%s): %s", clients[client_idx].id, current_time, msg);
            send_private_message(private_msg, client_idx, receiver_id);
        }
    }

    // STOP
    else if (strcmp(command, "STOP") == 0)
    {
        remove_client(client_idx);
    }
}

void broadcast_message(const char *msg, int client_idx)
{
    for (int i = 0; i < client_count; i++)
    {
        if (i != client_idx)
        {
            sendto(server_sockfd, msg, strlen(msg), 0, (struct sockaddr *)&clients[i].sockaddr, sizeof(clients[i].sockaddr));
        }
    }
}

void send_private_message(const char *msg, int client_idx, const char *receiver_id)
{
    for (int i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i].id, receiver_id) == 0)
        {
            sendto(server_sockfd, msg, strlen(msg), 0, (struct sockaddr *)&clients[i].sockaddr, sizeof(clients[i].sockaddr));
            break;
        }
    }
}

void remove_client(int client_idx)
{
    printf("Removing client: %s\n", clients[client_idx].id);
    clients[client_idx] = clients[client_count - 1];
    client_count--;
}
