#include <stdio.h>
#include "state.h"
#include "stateio.h"

int main()
{
    fprintf(stderr, "Running tests...\n");

    {
        char state_string[] = "xxxxx.o....o....o...xox.x";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != 1) {
            printf("invalid score for %s\n", state_string);
        }
    }

    {
        char state_string[] = "xo.x..o....o....o...xox.x";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != -1) {
            printf("invalid score for %s: %d\n", state_string, state_score(state));
        }
    }

    {
        char state_string[] = "oxoo..x....x....x...ox..o";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != 1) {
            printf("invalid score for %s: %d\n", state_string, state_score(state));
        }
    }

    {
        char state_string[] = "oxoo..x....o....x...ox..o";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != 0) {
            printf("invalid score for %s: %d\n", state_string, state_score(state));
        }
    }

    fprintf(stderr, "Done!\n");

    return 0;
}
