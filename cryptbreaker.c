#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>


#include <omp.h>
#include <mpi.h>
#include <crypt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define byte unsigned char

#define HASH_SIZE 14
#define VOCABULARY " ./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

int world_size = 1;
int world_rank = 0;

int vocabulary_size = 0;

char* data = NULL;
unsigned long data_size = 0;

char** hash_list = 0;
unsigned int hash_list_size = 0;

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
    if (is_frontend()) printf("vocabulary: %d |> '%s'\n", vocabulary_size, VOCABULARY);
}

void alloc_data() {
    data = malloc(sizeof(char)*(data_size + 1));
}

void broadcast_data() {
    MPI_Bcast(data, data_size, MPI_CHAR, root_id, MPI_COMM_WORLD);
}

void broadcast_data_size() {
    MPI_Bcast(&data_size, 1, MPI_UNSIGNED_LONG, root_id, MPI_COMM_WORLD);
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

void calcule_hash_list_size() {
    hash_list_size = (unsigned int)((data_size + 1) / HASH_SIZE);
}

void index_hash_list() {
    #pragma omp parallel for schedule(static)
    for(int index = 0; index < hash_list_size; index++) {
        hash_list[index] = &data[HASH_SIZE * index];
        hash_list[index][HASH_SIZE - 1] = '\0';
    }
}

void alloc_hash_list() {
    hash_list = (char**)malloc(sizeof(char*) * hash_list_size);
}

static int string_comparator(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b); 
} 
  
void sort(char** arr, int n) {
    qsort(arr, n, sizeof(const char*), string_comparator); 
}

void sort_hash_list() {
    sort(hash_list, hash_list_size);
}

void organize_data() {
    calcule_hash_list_size();
    alloc_hash_list();
    index_hash_list();
    sort_hash_list();
}

void break_it(char password[9], int thread_id) {
    char salt[2] = {'\0', '\0'};
    char* hash;

    struct crypt_data data;
    data.initialized = 0;

    for(int index; index < hash_list_size; index++) {
        char *indexed_hash_list = hash_list[index];
        if (indexed_hash_list[0] == '\0' ) continue;

        if (indexed_hash_list[0] != salt[0] || indexed_hash_list[1] != salt[1]) {
            salt[0] = indexed_hash_list[0];
            salt[1] = indexed_hash_list[1];
            hash = crypt_r(password, salt, &data);
        }

        
        if (indexed_hash_list[12] == hash[12] 
            && indexed_hash_list[11] == hash[11]
            && indexed_hash_list[10] == hash[10]
            && indexed_hash_list[9] == hash[9]
            && indexed_hash_list[8] == hash[8]
            && indexed_hash_list[7] == hash[7]
            && indexed_hash_list[6] == hash[6]
            && indexed_hash_list[5] == hash[5]
            && indexed_hash_list[4] == hash[4]
            && indexed_hash_list[3] == hash[3]
            && indexed_hash_list[2] == hash[2]) {
            indexed_hash_list[0] = '\0';
            printf("decrypted (THREAD %d RANK %d) |> %s |> %s\n", thread_id, world_rank, hash, password);
        }
    }
}

void combine_1() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0'};
        password[0] = VOCABULARY[n0];

        break_it(password, thread_id);
    }
}

void combine_2() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];
            break_it(password, thread_id);
        }
    }
}

void combine_3() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                break_it(password, thread_id);
            }
        }
    }
}

void combine_4() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                for (int n3 = 0; n3 < vocabulary_size; n3 ++) {
                    password[3] = VOCABULARY[n3];
                    break_it(password, thread_id);
                }
            }
        }
    }
}

void combine_5() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                for (int n3 = 0; n3 < vocabulary_size; n3 ++) {
                    password[3] = VOCABULARY[n3];
                    for (int n4 = 0; n4 < vocabulary_size; n4 ++) {
                        password[4] = VOCABULARY[n4];
                        break_it(password, thread_id);
                    }
                }
            }
        }
    }
}

void combine_6() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                for (int n3 = 0; n3 < vocabulary_size; n3 ++) {
                    password[3] = VOCABULARY[n3];
                    for (int n4 = 0; n4 < vocabulary_size; n4 ++) {
                        password[4] = VOCABULARY[n4];
                        for (int n5 = 0; n5 < vocabulary_size; n5 ++) {
                            password[5] = VOCABULARY[n5];
                            break_it(password, thread_id);
                        }
                    }
                }
            }
        }
    }
}

void combine_7() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                for (int n3 = 0; n3 < vocabulary_size; n3 ++) {
                    password[3] = VOCABULARY[n3];
                    for (int n4 = 0; n4 < vocabulary_size; n4 ++) {
                        password[4] = VOCABULARY[n4];
                        for (int n5 = 0; n5 < vocabulary_size; n5 ++) {
                            password[5] = VOCABULARY[n5];
                            for (int n6 = 0; n6 < vocabulary_size; n6 ++) {
                                password[6] = VOCABULARY[n6];
                                break_it(password, thread_id);
                            }
                        }
                    }
                }
            }
        }
    }
}

void combine_8() {
    #pragma omp parallel for
    for (int n0 = world_rank; n0 < vocabulary_size; n0 += world_size) {
        int thread_id = omp_get_thread_num();

        char password[9] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        password[0] = VOCABULARY[n0];

        for (int n1 = 0; n1 < vocabulary_size; n1 ++) {
            password[1] = VOCABULARY[n1];

            for (int n2 = 0; n2 < vocabulary_size; n2 ++) {
                password[2] = VOCABULARY[n2];
                for (int n3 = 0; n3 < vocabulary_size; n3 ++) {
                    password[3] = VOCABULARY[n3];
                    for (int n4 = 0; n4 < vocabulary_size; n4 ++) {
                        password[4] = VOCABULARY[n4];
                        for (int n5 = 0; n5 < vocabulary_size; n5 ++) {
                            password[5] = VOCABULARY[n5];
                            for (int n6 = 0; n6 < vocabulary_size; n6 ++) {
                                password[6] = VOCABULARY[n6];
                                for (int n7 = 0; n7 < vocabulary_size; n7 ++) {
                                    password[7] = VOCABULARY[n7];
                                    break_it(password, thread_id);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void combine() {
    combine_1();
    combine_2();
    combine_3();
    combine_4();
    combine_5();
    combine_6();
    combine_7();
    combine_8();
}

void front_end() {
    read_data();
    send_data();
    organize_data();
    combine();
}

void back_end() {
    recv_data();
    organize_data();
    combine();
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