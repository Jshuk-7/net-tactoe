#include "common.h"

#include <netinet/in.h>

const void* get_inet_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) // IPv4
    {
        return &((struct sockaddr_in*)sa)->sin_addr;
    }

    // IPv6
    return &((struct sockaddr_in6*)sa)->sin6_addr;
}