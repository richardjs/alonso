#include <stdio.h>
#include "state.h"
#include "stateio.h"

int main()
{
    char state_string[] = "..oo.xo.x.x.....";
    uint64_t state = state_from_string(state_string);
    uint64_t index = state_to_index(state);

    state_draw(state);
    fprintf(stderr, "\n");
    state_draw(state_from_index(index));

    return 0;
}
