#ifndef STATEIO_H
#define STATEIO_H


#define STATE_STRING_SIZE 26


void state_draw(uint64_t state);
void state_to_string(uint64_t state, char state_string[]);
uint64_t state_from_string(const char state_string[]);


#endif
