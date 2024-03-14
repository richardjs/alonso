#ifndef STATEIO_H
#define STATEIO_H


#define STATE_STRING_SIZE 26
#define ACTION_STRING_SIZE 5


void state_draw(uint64_t state);
void state_to_string(uint64_t state, char state_string[]);
uint64_t state_from_string(const char state_string[]);

uint8_t state_action_strings(uint64_t state, char action_strings[][ACTION_STRING_SIZE]);


#endif
