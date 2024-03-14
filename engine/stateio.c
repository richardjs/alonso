#include <stdint.h>
#include <stdio.h>
#include "stateio.h"


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
