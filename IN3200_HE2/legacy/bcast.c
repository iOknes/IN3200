#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 4

int main(int nargs, char **args) {
    int i, j;
    int rank, size;
    MPI_Init(&nargs, &args);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int **array = (int **) malloc(SIZE * sizeof(int*));
    array[0] = (int *) malloc(SIZE * SIZE * sizeof(int));
    for (i = 1; i < SIZE; i++) {
        array[i] = &(array[0][i * SIZE]);
    }

    if (rank == 0) {
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                array[i][j] = i * SIZE + j;
            }
        }
    }

    if (rank > 0) {
        int **array = (int **) malloc(SIZE * sizeof(int*));
        array[0] = (int *) malloc(SIZE * SIZE * sizeof(int));
        for (i = 1; i < SIZE; i++) {
            array[i] = &(array[0][i * SIZE]);
        }
    }

    MPI_Bcast(array[0], SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 2) {
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                printf("%4d ", array[i][j]);
            } putchar('\n');
        }
    }

    MPI_Finalize();
    return 0;
}