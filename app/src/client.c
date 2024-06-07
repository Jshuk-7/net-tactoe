#include "client.h"

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int32_t client_init(client_t* state, net_id_t netid)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* out = NULL;
    int32_t status = getaddrinfo(netid.domain, netid.port, &hints, &out);

    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    struct addrinfo* it = NULL;

    for (it = out; it != NULL; it = it->ai_next)
    {
        state->sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (state->sockfd == -1)
        {
            perror("socket");
            continue;
        }

        if (connect(state->sockfd, it->ai_addr, it->ai_addrlen) == -1)
        {
            perror("connect");
            close(state->sockfd);
            continue;
        }

        state->connected = true;

        break;
    }

    if (it == NULL)
    {
        fprintf(stderr, "client: failed to connect to host\n");
        freeaddrinfo(out);
        return 1;
    }

    freeaddrinfo(out);

    return 0;
}

void client_terminate(client_t* state)
{
    close(state->sockfd);
}

void client_run(client_t* state)
{
    char recvbuf[1024];

    size_t bytesread = recv(state->sockfd, recvbuf, sizeof recvbuf - 1, 0);
    recvbuf[bytesread] = '\0';
    printf("%s\n", recvbuf);
    
    while (state->connected)
    {
    }
}
