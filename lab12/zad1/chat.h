#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10
#define MAX_NAME_LEN 20

typedef struct
{
    struct sockaddr_in sockaddr;
    char id[MAX_NAME_LEN];
} Client;

// server
void remove_client(int);
void broadcast_message(const char *, int);
void send_private_message(const char *, int, const char *);
void handle_client_message(char *, int, int);
void get_current_time(char *, size_t);

// client
void cleanup();
void handle_signal(int);
void chat_client(const char *, int);
