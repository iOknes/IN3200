#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 4

int main(int argc, char *argv[])
{
    int rank, size;     // for storing this process' rank, and the number of processes
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *sendcounts = malloc(sizeof(int)*size);
    int *displs = malloc(sizeof(int)*size);

    int rem = (SIZE*SIZE)%size; // elements remaining after division among processes
    int sum = 0;                // Sum of counts. Used to calculate displacements
    char *rec_buf;          // buffer where the received data should be stored
    char **data;

    data = (char **) malloc(SIZE * sizeof(char*));
    data[0] = (char *) malloc(SIZE * SIZE * sizeof(char));
    for (int i = 1; i < SIZE; i++) {
        data[i] = &(data[0][i * SIZE]);
    }

    // the data to be distributed
    if (rank == 0) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                data[i][j] = 'a' + i * SIZE + j;
            }
        }
    }


    // calculate send counts and displacements
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (SIZE*SIZE)/size;
        if (rem > 0) {
            sendcounts[i]++;
            rem--;
        }

        displs[i] = sum;
        sum += sendcounts[i];
    }

    rec_buf = (char *) malloc(sendcounts[0] * sizeof(char));

    // print calculated send counts and displacements for each process
    if (0 == rank) {
        for (int i = 0; i < size; i++) {
            printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
        }
    }

    // divide the data among processes as described by sendcounts and displs
    MPI_Scatterv(data[0], sendcounts, displs, MPI_CHAR, rec_buf, SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

    // print what each process received
    printf("%d: ", rank);
    for (int i = 0; i < sendcounts[rank]; i++) {
        printf("%c\t", rec_buf[i]);
    }
    printf("\n");

    MPI_Finalize();

    free(sendcounts);
    free(displs);

    return 0;
}