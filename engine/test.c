#include <stdio.h>
#include "state.h"
#include "stateio.h"

int main()
{
    fprintf(stderr, "Running tests...\n");

    uint64_t children[MAX_ACTIONS];
    //uint8_t childrenc;

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
            printf("invalid score for %s: %f\n", state_string, state_score(state));
        }
    }

    {
        char state_string[] = "oxoo..x....x....x...ox..o";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != 1) {
            printf("invalid score for %s: %f\n", state_string, state_score(state));
        }
    }

    {
        char state_string[] = "oxoo..x....o....x...ox..o";
        uint64_t state = state_from_string(state_string);
        if (state_score(state) != 0) {
            printf("invalid score for %s: %f\n", state_string, state_score(state));
        }
    }

    {
        char state_string[] = "x..oox....x....x......o..";
        uint64_t state = state_from_string(state_string);
        if (state_has_win(state) < 0) {
            printf("failed to detect win for %s\n", state_string);
        }

        state_children(state, children);
        if (state_score(children[state_has_win(state)]) != -1.0) {
            printf("win is not win for %s\n", state_string);
        }
    }

    {
        char state_string[] = ".........................";
        uint64_t state = state_from_string(state_string);
        if (state_has_win(state) > 0) {
            printf("detected false win for %s\n", state_string);
        }
    }

    fprintf(stderr, "Done!\n");

    return 0;
}
