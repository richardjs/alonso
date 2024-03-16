#ifndef MCTS_H
#define MCTS_H


#include <stdbool.h>
#include "state.h"


#define DEFAULT_ITERATIONS 100000
#define DEFAULT_MAX_SIM_DEPTH 100
#define DEFAULT_UCTC 0.3
#define DEFAULT_SAVE_TREE false


struct Node {
    uint64_t state;
    uint64_t child_states[MAX_ACTIONS];
    uint8_t childrenc;

    bool expanded;
    unsigned int visits;
    float value;

    struct Node **children;

    uint16_t depth;
};


struct MCTSOptions {
    uint64_t iterations;
    float uctc;
    uint16_t max_sim_depth;
    bool save_tree;
};


struct MCTSStats {
    uint64_t iterations;
    uint64_t nodes;
    uint64_t tree_bytes;
    uint16_t tree_depth;
    uint32_t simulations;
    uint32_t early_sim_terminations;
    uint32_t tree_early_sim_terminations;
    uint32_t depth_outs;
    uint64_t duration;
};


struct MCTSResults {
    uint16_t actioni;
    float score;
    struct MCTSStats stats;
    struct Node nodes[MAX_ACTIONS];
    struct Node *tree;
};


void MCTSOptions_default(struct MCTSOptions*);


void mcts(uint64_t state, struct MCTSResults*, const struct MCTSOptions*);


#endif
