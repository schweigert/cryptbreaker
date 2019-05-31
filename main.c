#include <omp.h>
#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>

#include <crypt.h>

#define FRONT_ID 0

int world_size = 1;
int world_rank = 0;

void read_keys() {
    FILE* file;
    char* line = NULL;

    file = fopen("hash_list", "r");

    if (file  == NULL) {
        printf("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        line = (char*) malloc(sizeof(char) * 14);

        if (fscanf(file, "%s", line) == EOF) {
            fclose(file);
            return;
        }

        printf("%s\n", line);
    }
}

void setup() {
    read_keys();
}

void front() {}

void back() {}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("rank %d of %d\n", world_rank, world_size);

    setup();

    if (world_rank == FRONT_ID) {
        front();
    } else {
        back();
    }

    MPI_Finalize();
    return 0;
}