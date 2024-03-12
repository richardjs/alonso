#include <stdint.h>
#include <stdio.h>


// Tile layout:
//  0  1  2  3  4
//  5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24


#define MAX_CHILDREN (4*2 + 3*4*3)

#define EDGE_TILES 0b1111110001100011000111111
#define PUSH_UP_TILES 0b0000010001100011000111111
#define PUSH_DOWN_TILES 0b1111110001100011000100000
#define PUSH_LEFT_TILES 0b0111100001000010000101111
#define PUSH_RIGHT_TILES 0b1111010000100001000010000

#define bitscan(x) __builtin_ctz(x)
#define bitscanl(x) __builtin_clz(x)


uint64_t UP_B[25] = {
        0b100001000010000100001,
       0b1000010000100001000010,
      0b10000100001000010000100,
     0b100001000010000100001000,
    0b1000010000100001000010000,
        0b100001000010000100000,
       0b1000010000100001000000,
      0b10000100001000010000000,
     0b100001000010000100000000,
    0b1000010000100001000000000,
        0b100001000010000000000,
       0b1000010000100000000000,
      0b10000100001000000000000,
     0b100001000010000000000000,
    0b1000010000100000000000000,
        0b100001000000000000000,
       0b1000010000000000000000,
      0b10000100000000000000000,
     0b100001000000000000000000,
    0b1000010000000000000000000,
    0,
    0,
    0,
    0,
    0
};


uint64_t DOWN_B[25] = {
    0,
    0,
    0,
    0,
    0,
                       0b100001,
                      0b1000010,
                     0b10000100,
                    0b100001000,
                   0b1000010000,
                  0b10000100001,
                 0b100001000010,
                0b1000010000100,
               0b10000100001000,
              0b100001000010000,
             0b1000010000100001,
            0b10000100001000010,
           0b100001000010000100,
          0b1000010000100001000,
         0b10000100001000010000,
        0b100001000010000100001,
       0b1000010000100001000010,
      0b10000100001000010000100,
     0b100001000010000100001000,
    0b1000010000100001000010000
};

uint64_t LEFT_B[25] = {
                        0b11111,
                        0b11110,
                        0b11100,
                        0b11000,
    0,
                   0b1111100000,
                   0b1111000000,
                   0b1110000000,
                   0b1100000000,
    0,
              0b111110000000000,
              0b111100000000000,
              0b111000000000000,
              0b110000000000000,
    0,
         0b11111000000000000000,
         0b11110000000000000000,
         0b11100000000000000000,
         0b11000000000000000000,
    0,
    0b1111100000000000000000000,
    0b1111000000000000000000000,
    0b1110000000000000000000000,
    0b1100000000000000000000000,
    0
};

uint64_t RIGHT_B[25] = {
    0,
                           0b11,
                          0b111,
                         0b1111,
                        0b11111,
    0,
                      0b1100000,
                     0b11100000,
                    0b111100000,
                   0b1111100000,
    0,
                 0b110000000000,
                0b1110000000000,
               0b11110000000000,
              0b111110000000000,
    0,
            0b11000000000000000,
           0b111000000000000000,
          0b1111000000000000000,
         0b11111000000000000000,
    0,
       0b1100000000000000000000,
      0b11100000000000000000000,
     0b111100000000000000000000,
    0b1111100000000000000000000
};


void state_print(uint64_t state) {
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


int main() {
    uint64_t s = 0b0000000000000100011101010000010000000000011000100000100000001000;

    uint64_t children[MAX_CHILDREN];
    int childrenc = state_children(0, children);
    printf("%d\n", childrenc);

    for (int i = 0; i < childrenc; i++) {
        state_print(children[i]);
        getc(stdin);
    }
}
