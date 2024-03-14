#ifndef STATE_H
#define STATE_H


#include "bitboards.h"


#define MAX_CHILDREN (4*2 + 3*4*3)


#define STATE_STRING_SIZE 26


void state_draw(uint64_t state);
void state_to_string(uint64_t state, char state_string[]);
uint64_t state_from_string(const char state_string[]);

uint64_t state_swap_players(uint64_t state);
uint64_t state_open_tiles(uint64_t state);
uint8_t state_children(uint64_t state, uint64_t children[]);


#endif
