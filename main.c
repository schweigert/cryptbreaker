#include <omp.h>
#include <mpi.h>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>

#include "boom_file_read.h"

#define ROOT_ID 0
#define IMPUT_FN "hash_list"

int world_size = 1;
int world_rank = 0;

char* data = NULL;
unsigned long data_size = 0;

void read_data() {
    data = boom_file_read(IMPUT_FN);
    data_size = boom_file_size(IMPUT_FN);
}

void alloc_data() {
    data = malloc(sizeof(char)*(data_size + 1));
}

void broadcast_data() {
    MPI_Bcast(data, data_size, MPI_CHAR, world_rank, MPI_COMM_WORLD);
}

void broadcast_data_size() {
    MPI_Bcast(&data_size, 1, MPI_UNSIGNED_LONG, world_rank, MPI_COMM_WORLD);
}

void send_data() {
    broadcast_data_size();
    broadcast_data();
}

void recv_data() {
    broadcast_data_size();
    alloc_data();
    broadcast_data();
}

void front() {
    read_data();
    send_data();
}

void back() {
    recv_data();
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("rank %d of %d\n", world_rank, world_size);

    if (world_rank == ROOT_ID) {
        front();
    } else {
        back();
    }

    MPI_Finalize();
    return 0;
}