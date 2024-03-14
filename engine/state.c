#include <stdio.h>
#include "state.h"


#include "bitboards.h"


uint64_t state_swap_players(uint64_t state) {
    return state << 32 | state >> 32;
}


uint64_t state_open_tiles(uint64_t state) {
    return (EDGE_TILES & ~(state >> 32));
}


uint8_t state_children(uint64_t state, uint64_t children[]) {
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

            children[c++] = (((state & b) >> 5) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_DOWN_TILES) {
            uint8_t desti = tilei % 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = DOWN_B[tilei];
            b |= b << 32;

            children[c++] = (((state & b) << 5) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_LEFT_TILES) {
            uint8_t desti = (tilei / 5) * 5 + 4;
            uint64_t dest = 1llu << desti;

            uint64_t b = LEFT_B[tilei];
            b |= b << 32;

            children[c++] = (((state & b) >> 1) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_RIGHT_TILES) {
            uint8_t desti = (tilei / 5) * 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = RIGHT_B[tilei];
            b |= b << 32;

            children[c++] = (((state & b) << 1) & b) | (state & ~b) | dest;
        }
    }

    return c;
}
