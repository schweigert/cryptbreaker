#include <omp.h>
#include <mpi.h>

#include <stdio.h>
#include <crypt.h>
#include <stdlib.h>

#define FRONT_ID 0

int world_size = 1;
int world_rank = 0;


void front() {
}

void back() {

}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("rank %d of %d\n", world_rank, world_size);

    if (MPI_Comm_rank == FRONT_ID) {
        front();
    } else {
        back();
    }

    MPI_Finalize();
    return 0;
}