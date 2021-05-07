#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int nargs, char **args) {
    int my_rank, size;
    MPI_Init(&nargs, &args);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int i, j, m=4, n=4, temp_sum, remainder, cumulative_displacement=0;
    int **array, *sendcounts, *displs, *recvbuf;
    
    displs = (int *) malloc(size * sizeof(int));
    sendcounts = (int *) malloc(size * sizeof(int));
    
    array = (int **) malloc(m * sizeof(int *));
    array[0] = (int *) calloc(m * n, sizeof(int));
    for (i = 1; i < m; i ++) {
        array[i] = &(array[0][i * m]);
    }

    if (my_rank == 0) {
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                array[i][j] = i + j;
            }
        }
        
        printf("Printing array:\n");
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                printf("%4d ", array[i][j]);
            }
            putchar('\n');
        }

    }
    
    remainder = m % size;
    for (i = 0; i < size; i++) {
        sendcounts[i] = n;
        if (remainder > 0) {sendcounts[i] += n; remainder--;}
        displs[i] = cumulative_displacement;
        cumulative_displacement += sendcounts[i];
    }

    if (my_rank == 0) {    
        printf("Printing sendcounts:\n");
        for (i = 0; i < size; i++) {
            printf("%d ", sendcounts[i]);
        } putchar('\n');
        
        printf("Printing displs:\n");
        for (i = 0; i < size; i++) {
            printf("%d ", displs[i]);
        } putchar('\n');
    }

    recvbuf = (int *) malloc(sendcounts[my_rank] * sizeof(int));
    MPI_Scatterv(array[0], sendcounts, displs, MPI_INT, recvbuf, sendcounts[0], MPI_INT, 0, MPI_COMM_WORLD);

    for (i = 0; i < sendcounts[my_rank]; i++) {
        printf("%4d ", recvbuf[i]);
    } putchar('\n');

    for (i = 0; i < sendcounts[my_rank]; i++) {
        temp_sum += recvbuf[i];
    }

    printf("Thread %d sums to %d\n", my_rank, temp_sum);
    
    int *sums = malloc(m * sizeof(int));

    int *recvdispls = malloc(size * sizeof(int));
    int *recvcounts = malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        recvcounts[i] = sendcounts[i]/n;
    }
    recvdispls[0] = 0;
    for (i = 1; i < size; i++) {
        recvdispls[i] = recvdispls[i-1] + recvcounts[i-1];
    }

    MPI_Gatherv(&temp_sum, 1, MPI_INT, sums, recvcounts, recvdispls, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("-----------------------\nPrinting from thread %d:\n", my_rank);
        for (i = 0; i < m; i++) {
            printf("%d ", sums[i]);
        }
        putchar('\n');
    }
    
    free(recvbuf);
    MPI_Finalize();
 
    free(array[0]);
    free(array);
    free(sendcounts);
    free(displs);

    return 0;
}
