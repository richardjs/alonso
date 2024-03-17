#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "mcts.h"


// store these globally so we don't have to pass them around
static struct MCTSOptions options;
static struct MCTSResults *results;


/**
 * mallocs, checks for null, and increases results.stats.tree_bytes
 */
void *mctsmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "ERROR: failure to malloc in MCTS\n");
        exit(1);
    }
    results->stats.tree_bytes += size;
    return ptr;
}


void MCTSOptions_default(struct MCTSOptions *o)
{
        o->iterations = DEFAULT_ITERATIONS;
        o->uctc = DEFAULT_UCTC;
        o->max_sim_depth = DEFAULT_MAX_SIM_DEPTH;
        o->save_tree = DEFAULT_SAVE_TREE;
}


void Node_init(struct Node *node, uint64_t state, uint8_t depth)
{
    node->state = state;
    node->expanded = false;
    node->visits = 0;
    node->value = 0;
    // TODO we probably could pass this around mcts() and iterate()
    // instead of storing it here
    node->depth = depth;

    results->stats.nodes++;
    if (depth > results->stats.tree_depth) {
        results->stats.tree_depth = depth;
    }
}


/**
 * allocates space for children pointers and the child nodes themselves,
 * and calls Node_init on each child
 */
void Node_expand(struct Node *node)
{
    node->childrenc = state_children(node->state, node->child_states);
    node->children = mctsmalloc(sizeof(struct Node*) * node->childrenc);

    for (int i = 0; i < node->childrenc; i++) {
        node->children[i] = mctsmalloc(sizeof(struct Node));
        Node_init(node->children[i], node->child_states[i], node->depth+1);
    }

    node->expanded = true;
}


/**
 * frees the node's children along with the node
 */
void Node_free(struct Node *node)
{
    if (node->expanded) {
        for (int i = 0; i < node->childrenc; i++) {
            Node_free(node->children[i]);
        }
        free(node->children);
    }

    free(node);
}


/**
 * simulates play (in place) on a state, stopping at game end or
 * MAX_SIM_DEPTH, and returns 1.0 if the initial state won, -1.0 if it
 * lost, and 0.0 on a draw or depth out
 */
float simulate(uint64_t state)
{

    results->stats.simulations++;

    int turn = 1;

    uint64_t children[MAX_ACTIONS];
    uint8_t childrenc = state_children(state, children);

    int depth = 0;
    while (childrenc) {
        if (depth++ > options.max_sim_depth) {
            results->stats.depth_outs++;
            return 0.0;
        }

        if (state_has_win(state) >= 0) {
            return turn * 1.0;
        }

        state = children[rand() % childrenc];
        childrenc = state_children(state, children);
        turn *= -1.0;
    }

    return turn * state_score(state);
}


/**
 * single MCTS iteration: recursively walk down tree with state
 * (choosing promising children), simulate when we get to the end of the
 * tree, and update visited nodes with the results
 */
float iterate(struct Node *root)
{
    if (!root->expanded) {
        Node_expand(root);
    }

    // game-terminal node
    if (root->childrenc == 0) {
        root->visits++;
        root->value += -1.0;
        return state_score(root->state);
    }

    if (root->visits == 0) {
        float score = simulate(root->state);

        root->visits++;
        root->value += score;
        return score;
    }

    int childi = 0;
    float best_uct = -INFINITY;
    for (int i = 0; i < root->childrenc; i++) {
        if (root->children[i]->visits == 0) {
            childi = i;
            break;
        }

        float uct =
            -1*root->children[i]->value/root->children[i]->visits
            + options.uctc*sqrtf(logf(root->visits)/root->children[i]->visits);

        if (uct >= best_uct) {
            best_uct = uct;
            childi = i;
        }
    }

    struct Node *child = root->children[childi];

    float score = -1*iterate(child);

    root->visits++;
    root->value += score;
    return score;
}


void mcts(uint64_t state, struct MCTSResults *r, const struct MCTSOptions *o)
{
    results = r;
    memset(results, 0, sizeof(struct MCTSResults));

    if (o == NULL) {
        MCTSOptions_default(&options);
    } else {
        options = *o;
    }

    struct Node *root = mctsmalloc(sizeof(struct Node));
    Node_init(root, state, 0);
    Node_expand(root);

    if (root->childrenc == 0) {
        fprintf(stderr, "Can't run MCTS on state with no actions\n");
        return;
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < options.iterations; i++) {
        iterate(root);
        results->stats.iterations++;
    }

    struct timeval end;
    gettimeofday(&end, NULL);
    results->stats.duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results->score = -INFINITY;
    for (int i = 0; i < root->childrenc; i++) {
        float score = -1 * root->children[i]->value / root->children[i]->visits;

        if (score >= results->score) {
            results->score = score;
            results->actioni = i;
        }
    }

    for (int i = 0; i < root->childrenc; i++) {
        results->nodes[i] = *root->children[i];
    }

    if (options.save_tree) {
        results->tree = root;
    } else {
        Node_free(root);
        results->tree = NULL;
    }
}
