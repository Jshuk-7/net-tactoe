#pragma once

#include "common.h"

#define MAXCLIENTS 2
#define GAMEBOARD_WIDTH  3
#define GAMEBOARD_HEIGHT 3

typedef enum client_turn_t
{
    TURN_CLIENT1,
    TURN_CLIENT2,
} client_turn_t;

typedef struct game_state_t
{
    char board[GAMEBOARD_HEIGHT][GAMEBOARD_WIDTH];
    client_turn_t current_turn;
} game_state_t;

typedef struct server_t
{
    int32_t sockfd;
    client_t clients[MAXCLIENTS];
    
    game_state_t game_state;

    bool game_running;
} server_t;

int32_t server_init(server_t* state, net_id_t netid);
void server_terminate(server_t* state);
void server_launch(server_t* state);
char* server_render_game_board(server_t* state);
