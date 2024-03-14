#include <stdio.h>
#include "bitboards.h"
#include "state.h"
#include "stateio.h"


int main() {
    uint64_t s = 0b0000000000000100011101010000010000000000011000100000100000001000;

    uint64_t children[MAX_CHILDREN];
    int childrenc = state_children(0, children);
    printf("%d\n", childrenc);

    state_draw(s);

    char state_string[STATE_STRING_SIZE];
    state_to_string(s, state_string);
    printf("%s\n", state_string);

    state_draw(state_from_string(state_string));
}
