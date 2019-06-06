#include <omp.h>
#include <mpi.h>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>

#define HASH_SIZE 14
#define VOCABULARY "./0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM\0"

int world_size = 1;
int world_rank = 0;

int vocabulary_size = 0;

char* data = NULL;
int data_size = 0;

const int root_id = 0;

extern inline int count_vocabulary_size_is_ended() {
    return VOCABULARY[vocabulary_size] == '\0';
}

void count_vocabulary_size() {
    if (!count_vocabulary_size_is_ended()) {
        vocabulary_size++;
        count_vocabulary_size();
    }
}

int read_file_size(char* name) {
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fclose(file);

    return file_size;
}

char* read_all_file(char* name) {
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *large_buffer = malloc(sizeof(char)*(file_size + 1));
    fread(large_buffer, 1, file_size, file);
    fclose(file);

    large_buffer[file_size] = 0;

    return large_buffer;
}

void read_data() {
    data = read_all_file("imput");
    data_size = read_file_size("imput");
}

int is_frontend() {
    return world_rank == root_id;
}

void print_rank_status() {
    char* status = is_frontend() ? "frontend" : "backend";
    printf("rank %d of %d (is %s)\n", world_rank, world_size, status);
}

void print_vocabulary_status() {
    // printf("vocabulary: %d |> '%s'\n", vocabulary_size, VOCABULARY);
}

void alloc_data() {
    data = malloc(sizeof(char)*(data_size + 1));
}

void broadcast_data() {
    MPI_Bcast(data, data_size, MPI_CHAR, root_id, MPI_COMM_WORLD);
}

void broadcast_data_size() {
    MPI_Bcast(&data_size, 1, MPI_INT, root_id, MPI_COMM_WORLD);
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

void organize_data() {
    int hashes_count = (data_size + 1) / HASH_SIZE;
    printf("%lu %lu\n", data_size, hashes_count);
}

void front_end() {
    read_data();
    send_data();
    organize_data();
}

void back_end() {
    recv_data();
    organize_data();
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    count_vocabulary_size();

    print_rank_status();
    print_vocabulary_status();
    
    is_frontend() ? front_end() : back_end();

    MPI_Finalize();
    return 0;
}