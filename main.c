#include <stdio.h>
#include <crypt.h>
#include <stdlib.h>

#include "comparator.h"

int main(int argc, char** argv) {
    printf("%i |> %i\n", string_is_equal("ABCDEFGHIJKLMNO", "ABCDEFGHIJKLMNO"), 1 && 1 && 1 && 1 && 1);

    if (string_is_equal("ABCDEFGHIJKLMNO", "ABCDEFGHIJKLMNO")) {
        printf("Working!");
    } else {
        printf("Not working!");
    }
    return 0;
}