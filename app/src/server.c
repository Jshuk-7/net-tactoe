#include "server.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int32_t server_init(server_t* state, net_id_t netid)
{
	printf("starting server...\n");

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

		int32_t enabled = 1;
		if (setsockopt(state->sockfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int32_t)) == -1)
		{
			perror("setsockopt");
			close(state->sockfd);
			continue;
		}

		if (bind(state->sockfd, it->ai_addr, it->ai_addrlen) == -1)
		{
			perror("bind");
			close(state->sockfd);
			continue;
		}

		break;
	}

	if (it == NULL)
	{
		fprintf(stderr, "server: failed to bind to port: %s\n", netid.port);
		freeaddrinfo(out);
		return 1;
	}

	freeaddrinfo(out);

	state->game_running = false;
	
	for (size_t i = 0; i < MAXCLIENTS; i++)
	{
		state->clients[i].sockfd = 0;
		state->clients[i].connected = false;
	}

	for (size_t i = 0; i < GAMEBOARD_HEIGHT; i++)
	{
		for (size_t j = 0; j < GAMEBOARD_WIDTH; j++)
		{
			state->game_state.board[i][j] = ' ';
		}
	}

	state->game_state.current_turn = TURN_CLIENT1;

	return 0;
}

void server_terminate(server_t* state)
{
	printf("server: shutting down...\n");

	for (size_t i = 0; i < MAXCLIENTS; i++)
	{
		close(state->clients[i].sockfd);
	}

	close(state->sockfd);
}

void server_launch(server_t* state)
{
	if (listen(state->sockfd, MAXCLIENTS) == -1)
	{
		perror("listen");
		return;
	}

	printf("server: listening for connections...\n");

	struct sockaddr_storage conninfo;
	socklen_t connsize = sizeof conninfo;

	size_t connected_clients = 0;
	char ipstr[INET6_ADDRSTRLEN];

	// wait for all clients to connect
	while (connected_clients != MAXCLIENTS)
	{
		int32_t sockfd = accept(state->sockfd, (struct sockaddr*)&conninfo, &connsize);
		if (sockfd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(conninfo.ss_family, get_inet_addr((struct sockaddr*)&conninfo), ipstr, sizeof ipstr);
		printf("server: connection received from: '%s'\n", ipstr);

		for (size_t i = 0; i < MAXCLIENTS; i++)
		{
			client_t* client = &state->clients[i];

			if (client->connected)
				continue;
			
			client->sockfd = sockfd;
			client->connected = true;

			connected_clients++;
			break;
		}
	}

	printf("server: all clients connected. starting game session...\n");

	state->game_running = true;

	char* boardstr = server_render_game_board(state);
	size_t len = strlen(boardstr);

	for (size_t i = 0; i < MAXCLIENTS; i++)
	{
		client_t* client = &state->clients[i];

		size_t bytessent = send(client->sockfd, boardstr, len, 0);
	}

	while (state->game_running)
	{
		// process_input();
		// update();
		// render();
	}
}

/*
	x|o|x\n
	-----\n
	o|x|o\n
	-----\n
	o|x|o\0
*/
char* server_render_game_board(server_t* state)
{
	size_t height = GAMEBOARD_HEIGHT + (GAMEBOARD_HEIGHT - 1);
	size_t width = GAMEBOARD_WIDTH + (GAMEBOARD_WIDTH - 1);
    size_t len = height * width + 5;

	char* str = (char*)malloc(len * sizeof(char));

	size_t offset = 0;
	offset += snprintf(str + offset, len - offset, "%c|%c|%c\n",
		state->game_state.board[0][0], state->game_state.board[0][1], state->game_state.board[0][2]);
	offset += snprintf(str + offset, len - offset, "-----\n");
	offset += snprintf(str + offset, len - offset, "%c|%c|%c\n",
		state->game_state.board[1][0], state->game_state.board[1][1], state->game_state.board[1][2]);
	offset += snprintf(str + offset, len - offset, "-----\n");
	offset += snprintf(str + offset, len - offset, "%c|%c|%c\0",
		state->game_state.board[2][0], state->game_state.board[2][1], state->game_state.board[2][2]);

	return str;
}
