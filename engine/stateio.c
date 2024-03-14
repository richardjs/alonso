#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "state.h"
#include "stateio.h"


const char TILE_NAMES[25][3] = {
    "a1", "b1", "c1", "d1", "e1",
    "a2", "b2", "c2", "d2", "e2",
    "a3", "b3", "c3", "d3", "e3",
    "a4", "b4", "c4", "d4", "e4",
    "a5", "b5", "c5", "d5", "e5"
};


void state_draw(uint64_t state) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int tilei = i*5 + j;

            char c = '.';
            if ((1llu << tilei) & state) {
                c = 'X';
            }
            else if ((1llu << (tilei + 32)) & state) {
                c = 'O';
            }
            fputc(c, stderr);
        }
        fputc('\n', stderr);
    }
};


void state_to_string(uint64_t state, char state_string[]) {
    for (int i = 0; i < 25; i++) {
        if ((1llu << i) & state) {
            state_string[i] = 'x';
        } else if ((1llu << (i + 32)) & state) {
            state_string[i] = 'o';
        } else {
            state_string[i] = '.';
        }
    }
    state_string[25] = '\0';
}


uint64_t state_from_string(const char state_string[]) {
    uint64_t state = 0;
    for (int i = 0; i < 25; i++) {
        switch(state_string[i]) {
            case 'x':
                state |= (1llu << i);
                break;
            case 'o':
                state |= (1llu << (i + 32));
                break;
        }
    }
    return state;
}


void make_action_string(uint8_t tilei, uint8_t desti, char action_string[]) {
    strncpy(action_string, TILE_NAMES[tilei], 2);
    strncpy(&action_string[2], TILE_NAMES[desti], 2);
    action_string[4] = '\0';
}


// Actions will match the output of state_children
uint8_t state_action_strings(uint64_t state, char action_strings[][ACTION_STRING_SIZE]) {
    uint64_t open_tiles = state_open_tiles(state);

    uint8_t c = 0;
    while(open_tiles) {
        uint8_t tilei = bitscan(open_tiles);
        uint64_t tile = 1llu << tilei;

        open_tiles ^= tile;

        if (tile & PUSH_UP_TILES) {
            uint8_t desti = (tilei % 5) + 20;
            make_action_string(tilei, desti, action_strings[c++]);
        }

        if (tile & PUSH_DOWN_TILES) {
            uint8_t desti = tilei % 5;
            make_action_string(tilei, desti, action_strings[c++]);
        }

        if (tile & PUSH_LEFT_TILES) {
            uint8_t desti = (tilei / 5) * 5 + 4;
            make_action_string(tilei, desti, action_strings[c++]);
        }

        if (tile & PUSH_RIGHT_TILES) {
            uint8_t desti = (tilei / 5) * 5;
            make_action_string(tilei, desti, action_strings[c++]);
        }
    }

    return c;
}
