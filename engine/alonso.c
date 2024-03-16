#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bitboards.h"
#include "errors.h"
#include "state.h"
#include "stateio.h"


#define VERSION ".1a"


enum Command {
    NONE,
    LIST_ACTIONS,
    ACT
};


int main(int argc, char* argv[])
{
    fprintf(stderr, "Alonso v%s (built %s %s)\n", VERSION, __DATE__, __TIME__);

    enum Command command = NONE;
    char state_string[STATE_STRING_SIZE];
    char *action;

    int opt;
    while ((opt = getopt(argc, argv, "a:lv")) != -1) {
        switch(opt) {
        case 'a':
            command = ACT;
            action = optarg;
            break;

        case 'l':
            command = LIST_ACTIONS;
            break;

        case 'v':
            return 0;
        }
    }

    if (argc == optind) {
        fprintf(stderr, "No state provided\n");
        exit(ERROR_NO_STATE_GIVEN);
    }

    uint64_t state = state_from_string(argv[optind]);
    fprintf(stderr, "Input state: %s\n", argv[optind]);
    state_draw(state);

    uint64_t children[MAX_ACTIONS];
    int c = state_children(state, children);

    char actions[MAX_ACTIONS][ACTION_STRING_SIZE];
    state_action_strings(state, actions);

    switch(command) {
    case NONE:
        fprintf(stderr, "No command provided\n");
        exit(ERROR_NO_COMMAND);

    case LIST_ACTIONS:
        if (state_score(state) != 0) {
            fprintf(stderr, "Terminal state\n");
            break;
        }
        for (int i = 0; i < c; i++) {
            printf("%s\n", actions[i]);
        }
        break;

    case ACT:
        for (int i = 0; i < c; i++) {
            if (strcmp(action, actions[i]) == 0) {
                state = children[i];
                state_to_string(state, state_string);
                fprintf(stderr, "Output state: %s\n", state_string);
                state_draw(state);
                printf("%s\n", state_string);
                goto command_done;
            }
        }
        fprintf(stderr, "Invalid action");
        exit(ERROR_INVALID_ACTION);
    }

    command_done:

    return 0;
}
