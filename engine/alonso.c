#include <stdio.h>
#include "bitboards.h"
#include "state.h"


int main() {
    //uint64_t s = 0b0000000000000100011101010000010000000000011000100000100000001000;

    uint64_t children[MAX_CHILDREN];
    int childrenc = state_children(0, children);
    printf("%d\n", childrenc);
}
