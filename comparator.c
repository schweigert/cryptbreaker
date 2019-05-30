#include "comparator.h"

#include <stdio.h>

int string_is_equal(char* hash1, char* hash2) {
    return hash1[0] == hash2[0]
        && hash1[1] == hash2[1]
        && hash1[2] == hash2[2]
        && hash1[3] == hash2[3]
        && hash1[4] == hash2[4]
        && hash1[5] == hash2[5]
        && hash1[6] == hash2[6]
        && hash1[7] == hash2[7]
        && hash1[8] == hash2[8]
        && hash1[9] == hash2[9]
        && hash1[10] == hash2[10]
        && hash1[11] == hash2[11]
        && hash1[12] == hash2[12];
}
