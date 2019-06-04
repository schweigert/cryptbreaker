#include <omp.h>
#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>

#include <crypt.h>

#define FRONT_ID 0

int world_size = 1;
int world_rank = 0;

struct Imput {
    char* hash;
    struct Imput* next;
};

struct Imput* NewImput(char* hash, struct Imput* next) {
    struct Imput* imput = (struct Imput*)malloc(sizeof(struct Imput));

    imput->next = next;
    imput->hash = hash;

    return imput;
}

struct Imput* imput_list = NULL;

char* read_line(FILE* file) {
        char *line = (char*) malloc(sizeof(char) * 14);

        if (fscanf(file, "%s", line) == EOF) {
            return NULL;
        }

        return line;
}

void print_recursive_imput_list(struct Imput* imput) {
    if (imput == NULL) {
        return;
    }

    printf("%s\n", imput->hash);
    print_recursive_imput_list(imput->next);
}

void print_imput_list() {
    print_recursive_imput_list(imput_list);
}

void import_imput_list() {
    FILE* file;
    char* line = NULL;

    file = fopen("hash_list", "r");

    if (file  == NULL) {
        printf("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        line = read_line(file);

        if (line == NULL) {
            fclose(file);
            return;
        }

        imput_list = NewImput(line, imput_list);
    }
}

void front() {
    import_imput_list();
    print_imput_list();
}

void back() {}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("rank %d of %d\n", world_rank, world_size);

    if (world_rank == FRONT_ID) {
        front();
    } else {
        back();
    }

    MPI_Finalize();
    return 0;
}