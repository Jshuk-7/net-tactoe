#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>

typedef struct client_t
{
    int32_t sockfd;
    pthread_t recv_thread;
    bool connected;
} client_t;

typedef struct net_id_t
{
    char domain[128];
    char port[64];
} net_id_t;

const void* get_inet_addr(struct sockaddr* sa);
