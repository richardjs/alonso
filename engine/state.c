#include <stdio.h>
#include "state.h"
#include "bitboards.h"


uint64_t state_swap_players(uint64_t state)
{
    return state << 32 | state >> 32;
}


uint64_t state_open_tiles(uint64_t state)
{
    return (EDGE_TILES & ~(state >> 32));
}


uint8_t state_children(uint64_t state, uint64_t children[])
{
    uint64_t open_tiles = state_open_tiles(state);

    uint8_t c = 0;
    while(open_tiles) {
        uint8_t tilei = bitscan(open_tiles);
        uint64_t tile = 1llu << tilei;

        open_tiles ^= tile;

        if (tile & PUSH_UP_TILES) {
            uint8_t desti = (tilei % 5) + 20;
            uint64_t dest = 1llu << desti;

            uint64_t b = UP_B[tilei];
            b |= b << 32;

            children[c++] = state_swap_players(
                (((state & b) >> 5) & b) | (state & ~b) | dest
            );
        }

        if (tile & PUSH_DOWN_TILES) {
            uint8_t desti = tilei % 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = DOWN_B[tilei];
            b |= b << 32;

            children[c++] = state_swap_players(
                (((state & b) << 5) & b) | (state & ~b) | dest
            );
        }

        if (tile & PUSH_LEFT_TILES) {
            uint8_t desti = (tilei / 5) * 5 + 4;
            uint64_t dest = 1llu << desti;

            uint64_t b = LEFT_B[tilei];
            b |= b << 32;

            children[c++] = state_swap_players(
                (((state & b) >> 1) & b) | (state & ~b) | dest
            );
        }

        if (tile & PUSH_RIGHT_TILES) {
            uint8_t desti = (tilei / 5) * 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = RIGHT_B[tilei];
            b |= b << 32;

            children[c++] = state_swap_players(
                (((state & b) << 1) & b) | (state & ~b) | dest
            );
        }
    }

    return c;
}


int8_t state_score(uint64_t state)
{
    for (int i = 0; i < NUM_WINS; i++) {
        if ((state & O_WINS[i]) == O_WINS[i]) {
            return -1;
        }
    }

    for (int i = 0; i < NUM_WINS; i++) {
        if ((state & X_WINS[i]) == X_WINS[i]) {
            return 1;
        }
    }

    return 0;
}


uint64_t state_from_index(uint64_t index)
{
    uint64_t state = 0;

    int tilei = 0;
    while (index > 0) {
        switch(index % 3) {
            case 1:
                state |= (1llu << tilei);
                break;
            case 2:
                state |= (1llu << (tilei + 32));
                break;
        }
        index /= 3;
        tilei++;
    }

    return state;
}


uint64_t intpow(uint64_t n, int e)
{
    int x = 1;
    for (int i = 0; i < e; i++){
        x *= n;
    }
    return x;
}


uint64_t state_to_index(uint64_t state)
{
    uint64_t index = 0;
    for (int i = 0; i < 25; i++) {
        if (state & (1llu << i)) {
            index += 1 * intpow(3, i);
        } else if (state & (1llu << (i+32))) {
            index += 2 * intpow(3, i);
        }
    }
    return index;
}
