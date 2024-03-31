#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "bitboards.h"
#include "errors.h"
#include "mcts.h"
#include "state.h"
#include "stateio.h"


#define VERSION ".1a"


#define TOP_ACTIONS 5


enum Command {
    ACT,
    LIST_ACTIONS,
    NONE,
    THINK
};


int main(int argc, char* argv[])
{
    fprintf(stderr, "Alonso v%s (built %s %s)\n", VERSION, __DATE__, __TIME__);

    enum Command command = NONE;
    char state_string[STATE_STRING_SIZE];
    char *action;

    uint8_t workers = 1;

    struct MCTSOptions options;
    MCTSOptions_default(&options);

    int opt;
    while ((opt = getopt(argc, argv, "a:li:tvw:")) != -1) {
        switch(opt) {
        case 'a':
            command = ACT;
            action = optarg;
            break;

        case 'i':
            options.iterations = atoi(optarg);
            break;

        case 'l':
            command = LIST_ACTIONS;
            break;

        case 't':
            command = THINK;
            break;

        case 'v':
            return 0;

        case 'w':
            workers = atoi(optarg);
            break;
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
    int childrenc = state_children(state, children);

    char actions[MAX_ACTIONS][ACTION_STRING_SIZE];
    state_action_strings(state, actions);

    time_t seed = time(NULL);
    srand(seed);

    switch(command) {
    case NONE:
        fprintf(stderr, "No command provided\n");
        exit(ERROR_NO_COMMAND);

    case LIST_ACTIONS:
        if (state_score(state) != 0) {
            fprintf(stderr, "Terminal state\n");
            goto command_done;
        }
        for (int i = 0; i < childrenc; i++) {
            printf("%s\n", actions[i]);
        }
        goto command_done;

    case ACT:
        for (int i = 0; i < childrenc; i++) {
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

    case THINK:
    }

    if (state_score(state) != 0) {
        fprintf(stderr, "Terminal state\n");

        if (state_score(state) == 1) {
            fprintf(stderr, "result: win\n");
        } else if (state_score(state) == -1) {
            fprintf(stderr, "result: loss\n");
        }

        goto command_done;
    }

    if (state_has_win(state) > 0) {
        fprintf(stderr, "Taking win\n");
        fprintf(stderr, "result: win\n");
        state_draw(children[state_has_win(state)]);
        state_to_string(children[state_has_win(state)], state_string);
        printf("%s\n", state_string);
        goto command_done;
    }

    int pipefd[2];
    pipe(pipefd);

    for (int i = 0; i < workers; i++) {
        srand(rand());
        if (fork() > 0) {
            continue;
        }
        struct MCTSResults results;
        mcts(state, &results, &options);
        write(pipefd[1], &results, sizeof(struct MCTSResults));
        exit(0);
    }

    struct MCTSResults results;
    memset(&results, 0, sizeof(struct MCTSResults));

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < workers; i++) {
        wait(NULL);

        struct MCTSResults worker_results;
        read(pipefd[0], &worker_results, sizeof(struct MCTSResults));

        for (int j = 0; j < childrenc; j++) {
            results.nodes[j].visits += worker_results.nodes[j].visits;
            results.nodes[j].value += worker_results.nodes[j].value;
        }

        results.stats.iterations += worker_results.stats.iterations;
        results.stats.nodes += worker_results.stats.nodes;
        results.stats.tree_bytes += worker_results.stats.tree_bytes;
        results.stats.simulations += worker_results.stats.simulations;
        results.stats.depth_outs += worker_results.stats.depth_outs;
    }

    struct timeval end;
    gettimeofday(&end, NULL);
    results.stats.duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results.score = -INFINITY;
    int top_actionis[TOP_ACTIONS];
    memset(top_actionis, -1, sizeof(int) * TOP_ACTIONS);
    for (int i = 0; i < childrenc; i++) {
        float score = -1 * results.nodes[i].value / results.nodes[i].visits;

        if (score >= results.score) {
            results.score = score;
            results.actioni = i;
        }

        for (int j = 0; j < TOP_ACTIONS; j++) {
            if (top_actionis[j] < 0) {
                top_actionis[j] = i;
                break;
            }

            float s = -1 * results.nodes[top_actionis[j]].value / results.nodes[top_actionis[j]].visits;
            if (score > s) {
                for (int k = TOP_ACTIONS - 2; k >= j ; k--) {
                    top_actionis[k+1] = top_actionis[k];
                }
                top_actionis[j] = i;
                break;
            }
        }
    }

    fprintf(stderr, "action:\t\t%s\n", actions[results.actioni]);
    fprintf(stderr, "score:\t\t%.2f\n", results.score);

    fprintf(stderr, "iterations:\t%ld\n", results.stats.iterations);
    fprintf(stderr, "workers:\t%d\n", workers);
    fprintf(stderr, "time:\t\t%ld ms\n", results.stats.duration);
    fprintf(stderr, "iters/s:\t%ld\n",
        results.stats.duration ?
            1000 * results.stats.iterations / results.stats.duration : 0);
    fprintf(stderr, "tree size:\t%ld MiB\n",
        results.stats.tree_bytes / 1024 / 1024);
    fprintf(stderr, "depth outs:\t%.2f%%\n",
        100.0*results.stats.depth_outs/results.stats.iterations);

    for (int i = 1; i < TOP_ACTIONS; i++) {
        float score = -1 * results.nodes[top_actionis[i]].value / results.nodes[top_actionis[i]].visits;
        fprintf(stderr, "alt:\t%.2f\t%s\n", score, actions[top_actionis[i]]);
    }

    uint64_t after = children[results.actioni];
    state_draw(after);

    state_to_string(after, state_string);
    printf("%s\n", state_string);
    fprintf(stderr, "next:\t%s\n", state_string);

    command_done:
    return 0;
}
