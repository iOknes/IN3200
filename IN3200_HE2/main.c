#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "MPI_single_layer_convolution.c"

int main (int nargs, char **args) {
	int i, j, k;
    int M = 0, N = 0, K = 0, my_rank, size, n_jobs, remainder, tot_displ;
    float **input = NULL, **output = NULL, **kernel = NULL;

	n_jobs = M - K + 1;

    MPI_Init(&nargs, &args);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
		// read from command line the values of M, N, and K
		for (i = 0; i < nargs; i++) {
			printf("%s\n", args[i]);
		}
		if (nargs == 4) {
            // If supplied with run command, use the first three args
            M = atoi(args[1]);
            N = atoi(args[2]);
            K = atoi(args[3]);
        } else {
            // Otherwise, read one by one as input with prompts
            printf("Enter value for M:\n");
            scanf("%d", &M);
            printf("Enter value for N:\n");
            scanf("%d", &N);
            printf("Enter value for K:\n");
            scanf("%d", &K);
        }
		// allocate 2D array ’input’ with M rows and N columns
		input = (float **) malloc(M * sizeof(float *));
        // Using calloc to assign inner array to auto init as 0
        input[0] = (float *) calloc(M * N, sizeof(float));
        for (i = 1; i < M; i++) {
            input[i] = &(input[0][i * N]);
        }
		// allocate 2D array ’output’ with M-K+1 rows and N-K+1 columns
		output = (float **) malloc((M-K+1) * sizeof(float *));
        output[0] = (float *) calloc((M-K+1) * (N-K+1), sizeof(float));
        for (i = 1; i <= M-K; i++) {
            output[i] = &(output[0][i * (N-K+1)]);
        }
		// allocate the convolutional kernel with K rows and K columns
		kernel = (float **) malloc(K * sizeof(float *));
        kernel[0] = (float *) calloc(K * K, sizeof(float));
        for (i = 1; i < K; i++) {
            kernel[i] = &(kernel[0][i * K]);
        }
		// fill 2D array ’input’ with some values
		for (i = 0; i < M; i++) {
            for (j = 0; j < N; j++) {
                if (j < 4) {
                    input[i][j] = 10;
                }
            }
        }
		// fill kernel with some values
		for (i = 0; i < K; i++) {
            for (j = 0; j < K; j++) {
                if (j == 0) {
                    kernel[i][j] = 1;
                }
                if (j == K-1) {
                    kernel[i][j] = -1;
                }
            }
        }
		// ....
	}
	// process 0 broadcasts values of M, N, K to all the other processes
	// ...
	MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//printf("M, N, K have been broadcast on thread %d...\n", my_rank);

	if (my_rank>0) {
		// allocated the convolutional kernel with K rows and K columns
		// ...
		kernel = (float **) malloc(K * sizeof(float *));
        kernel[0] = (float *) calloc(K * K, sizeof(float));
        for (i = 1; i < K; i++) {
            kernel[i] = &(kernel[0][i * K]);
        }
	}
	// process 0 broadcasts the content of kernel to all the other processes
	// ...
	MPI_Bcast(kernel[0], K * K, MPI_FLOAT, 0, MPI_COMM_WORLD);
	// parallel computation of a single-layer convolution
	MPI_single_layer_convolution (M, N, input, K, kernel, output);
	if (my_rank == 0) {
		float **singlethread_output;
		int comparison = 0;
		// For example, compare the content of array ’output’ with that is
		// produced by the sequential function single_layer_convolution
		// ... 
		singlethread_output = (float **) malloc((M-K+1) * sizeof(float *));
        singlethread_output[0] = (float *) calloc((M-K+1) * (N-K+1), sizeof(float));
        for (i = 1; i <= M-K; i++) {
            singlethread_output[i] = &(singlethread_output[0][i * (N-K+1)]);
        }
		
		single_layer_convolution(M, N, input, K, kernel, singlethread_output);
		
		for (i = 0; i < (M-K+1); i++) {
			for (j = 0; j < (N-K+1); j++) {
				//printf("Comparing [%d][%d] on thread %d...\n", i, j, my_rank);
				comparison += singlethread_output[i][j] == output[i][j];
			}
		}
		printf("Outputs compared on thread %d. Result: %d!\n", my_rank, comparison);
	}
    MPI_Finalize();
	return 0;
}
