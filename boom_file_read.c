#include <stdio.h>
#include <stdlib.h>

#include "boom_file_read.h"

unsigned long boom_file_size(char* name) {
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    unsigned long file_size = ftell(file);
    fclose(file);

    return file_size;
}

char* boom_file_read(char* name) {
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    unsigned long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *large_buffer = malloc(sizeof(char)*(file_size + 1));
    fread(large_buffer, 1, file_size, file);
    fclose(file);

    large_buffer[file_size] = 0;

    return large_buffer;
}