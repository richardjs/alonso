#include <stdio.h>
#include "state.h"


#include "bitboards.h"


void state_draw(uint64_t state) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int tilei = i*5 + j;

            char c = '.';
            if (((1llu << tilei) & state) > 0) {
                c = 'X';
            }
            else if (((1llu << (tilei + 32)) & state) > 0) {
                c = 'O';
            }
            fputc(c, stderr);
        }
        fputc('\n', stderr);
    }
};


uint64_t state_swap_players(uint64_t state) {
    return state << 32 | state >> 32;
}


uint64_t state_open_tiles(uint64_t state) {
    return (EDGE_TILES & ~(state >> 32));
}


uint8_t state_children(uint64_t state, uint64_t children[]) {
    uint64_t open_tiles = state_open_tiles(state);

    uint8_t childrenc = 0;
    while(open_tiles) {
        uint8_t tilei = bitscan(open_tiles);
        uint64_t tile = 1llu << tilei;

        open_tiles ^= tile;

        if (tile & PUSH_UP_TILES) {
            uint8_t desti = (tilei % 5) + 20;
            uint64_t dest = 1llu << desti;

            uint64_t b = UP_B[tilei];
            b |= b << 32;

            children[childrenc++] = (((state & b) >> 5) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_DOWN_TILES) {
            uint8_t desti = tilei % 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = DOWN_B[tilei];
            b |= b << 32;

            children[childrenc++] = (((state & b) << 5) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_LEFT_TILES) {
            uint8_t desti = (tilei / 5) * 5 + 4;
            uint64_t dest = 1llu << desti;

            uint64_t b = LEFT_B[tilei];
            b |= b << 32;

            children[childrenc++] = (((state & b) >> 1) & b) | (state & ~b) | dest;
        }

        if (tile & PUSH_RIGHT_TILES) {
            uint8_t desti = (tilei / 5) * 5;
            uint64_t dest = 1llu << desti;

            uint64_t b = RIGHT_B[tilei];
            b |= b << 32;

            children[childrenc++] = (((state & b) << 1) & b) | (state & ~b) | dest;
        }
    }

    return childrenc;
}
