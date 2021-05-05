#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int nargs, char **args) {
    int i, j, m=6, n=4, temp_sum, remainder, cumulative_displacement=0;
    int **array, *sendcounts, *displs, *recvbuf;

    int my_rank, size;
    MPI_Init(&nargs, &args);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    if (my_rank == 0) {
        displs = (int *) malloc(size * sizeof(int));
        sendcounts = (int *) malloc(size * sizeof(int));

        array = (int **) malloc(m * sizeof(int *));
        array[0] = (int *) calloc(m * n, sizeof(int));
        for (i = 1; i < m; i ++) {
            array[i] = &(array[0][i * m]);
        }

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

        remainder = m % size;
        for (i = 0; i < size; i++) {
            sendcounts[i] = n;
            if (remainder > 0) {sendcounts[i] += n; remainder--;}
            displs[i] = cumulative_displacement;
            cumulative_displacement += sendcounts[i];
        }

        printf("Printing sendcounts:\n");
        for (i = 0; i < size; i++) {
            printf("%d ", sendcounts[i]);
        } putchar('\n');
        
        printf("Printing displs:\n");
        for (i = 0; i < size; i++) {
            printf("%d ", displs[i]);
        } putchar('\n');
    }

    printf("%d\n", my_rank);
    recvbuf = (int *) malloc(sendcounts[0] * sizeof(int));
    MPI_Scatterv(array[0], sendcounts, displs, MPI_INT, recvbuf, sendcounts[0], MPI_INT, 0, MPI_COMM_WORLD);
    printf("Thread %d: ", my_rank);

    //printf("Hello from thread %d :)\n", my_rank);
    /*for (i = 0; i < sendcounts[my_rank]; i++) {
        printf("%d ", recvbuf[i]);
    } putchar('\n');*/

    for (i = 0; i < sendcounts[my_rank]; i++) {
        temp_sum += recvbuf[i];
    }
    
    /*if (my_rank == 0) {
        for (i = 0; i < m; )
    }*/
    
    free(recvbuf);
    MPI_Finalize();
 
    free(array[0]);
    free(array);
    free(sendcounts);
    free(displs);

    return 0;
}
