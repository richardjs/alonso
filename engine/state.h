#ifndef STATE_H
#define STATE_H


#include "bitboards.h"


#define MAX_ACTIONS (4*2 + 3*4*3)


uint64_t state_swap_players(uint64_t state);
uint64_t state_open_tiles(uint64_t state);


uint8_t state_children(uint64_t state, uint64_t children[]);


int8_t state_score(uint64_t state);


uint64_t state_from_index(uint64_t index);
uint64_t state_to_index(uint64_t state);


#endif
