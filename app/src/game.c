#include <stdio.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "common.h"

int main(int argc, char** argv)
{
    uint32_t choice;
    net_id_t netid;

    while (choice != 1 && choice != 2)
    {
        printf("Start a server or join as a client?\n");
	    printf("1. server\n2. client\n>> ");
        scanf("%u", &choice);
    }

    printf("Enter: <ipaddr> <port>\n>> ");
    scanf("%s %s", netid.domain, netid.port);
    printf("\n");

    if (choice == 1) // server
    {
        server_t server;
        
        if (server_init(&server, netid) != 0)
            return 1;

        server_launch(&server);
        server_terminate(&server);
    }
    else // client
    {
        client_t client;

        if (client_init(&client, netid) != 0)
            return 1;

        client_run(&client);
        client_terminate(&client);
    }
}