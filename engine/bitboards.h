#ifndef BITBOARDS_H
#define BITBOARDS_H


#include <stdint.h>


// Tile layout:
//  0  1  2  3  4
//  5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24


#define EDGE_TILES          0b1111110001100011000111111


// Tiles from which each move is possible
#define PUSH_UP_TILES       0b0000010001100011000111111
#define PUSH_DOWN_TILES     0b1111110001100011000100000
#define PUSH_LEFT_TILES     0b0111100001000010000101111
#define PUSH_RIGHT_TILES    0b1111010000100001000011110


// The B constants refer to the B term in Tanaka, et al.
extern const uint64_t UP_B[25];
extern const uint64_t DOWN_B[25];
extern const uint64_t LEFT_B[25];
extern const uint64_t RIGHT_B[25];


#endif
