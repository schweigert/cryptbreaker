#include <omp.h>
#include <mpi.h>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT_ID 0
#define IMPUT_FN "hash_list"
#define VOCABULARY "./0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM\0"

int world_size = 1;
int world_rank = 0;

int vocabulary_size = 0;

char* data = NULL;
unsigned long data_size = 0;

extern inline int count_vocabulary_size_is_ended() {
    return VOCABULARY[vocabulary_size] == '\0';
}

void count_vocabulary_size() {
    if (!count_vocabulary_size_is_ended()) {
        vocabulary_size++;
        count_vocabulary_size();
    }
}

unsigned long read_file_size(char* name) {
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    unsigned long file_size = ftell(file);
    fclose(file);

    return file_size;
}

char* read_all_file(char* name) {
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

void read_data() {
    data = read_all_file(IMPUT_FN);
    data_size = read_file_size(IMPUT_FN);
}

void print_rank_status() {
    printf("rank %d of %d\n", world_rank, world_size);
}

void print_vocabulary_status() {
    printf("vocabulary: %d |> '%s'\n", vocabulary_size, VOCABULARY);
}

int is_frontend() {
    return world_rank == ROOT_ID;
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

void front_end() {
    read_data();
    send_data();
}

void back_end() {
    recv_data();
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