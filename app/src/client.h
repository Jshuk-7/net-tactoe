#pragma once

#include "common.h"

int32_t client_init(client_t* state, net_id_t netid);
void client_terminate(client_t* state);
void client_run(client_t* state);
