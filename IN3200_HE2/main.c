#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include "single_layer_convolution.c"
#include "MPI_single_layer_convolution.c"

int main (int nargs, char **args) {
	int i, j, k;
    int M = 0, N = 0, K = 0, my_rank, size;
    float **input = NULL, **output = NULL, **kernel = NULL;

    time_t mp_start, mp_end, sp_start, sp_end;
    int sp_time, mp_time;

    MPI_Init(&nargs, &args);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
		// read from command line the values of M, N, and K
		if (nargs == 4) {
            // If supplied with run command, use the first three args
            M = atoi(args[1]);
            N = atoi(args[2]);
            K = atoi(args[3]);
        } else {
            // Otherwise, read one by one as input with prompts
            printf("Enter values for M, N, K:\n");
            scanf("%d%d%d", &M, &N, &K);
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
	}

	// process 0 broadcasts values of M, N, K to all the other processes
	MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (my_rank > 0) {
		// allocated the convolutional kernel with K rows and K columns
		kernel = (float **) malloc(K * sizeof(float *));
        kernel[0] = (float *) calloc(K * K, sizeof(float));
        for (i = 1; i < K; i++) {
            kernel[i] = &(kernel[0][i * K]);
        }
	}
	
    // process 0 broadcasts the content of kernel to all the other processes
    MPI_Bcast(kernel[0], K * K, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// parallel computation of a single-layer convolution
	if (my_rank == 0) {mp_start = time(NULL);}
	MPI_single_layer_convolution (M, N, input, K, kernel, output);
	if (my_rank == 0) {mp_end = time(NULL);}

	if (my_rank == 0) {
		float **singlethread_output;
		int comparison = 0;

		// For example, compare the content of array ’output’ with that is
		// produced by the sequential function single_layer_convolution
		singlethread_output = (float **) malloc((M-K+1) * sizeof(float *));
        singlethread_output[0] = (float *) calloc((M-K+1) * (N-K+1), sizeof(float));
        for (i = 1; i <= M-K; i++) {
            singlethread_output[i] = &(singlethread_output[0][i * (N-K+1)]);
        }
		
        sp_start = time(NULL);
		single_layer_convolution(M, N, input, K, kernel, singlethread_output);
        sp_end = time(NULL);
		
		for (i = 0; i < (M-K+1); i++) {
			for (j = 0; j < (N-K+1); j++) {
				comparison += singlethread_output[i][j] == output[i][j];
			}
		}
		printf("Comparing outputs: %d/%d matches.\n", comparison, (M-K+1) * (N-K+1));
        sp_time = (int) (sp_end - sp_start);
        mp_time = (int) (mp_end - mp_start);
		printf("Calculation time (RTC):\nSinglethread: %6d Multithread: %6d\n", sp_time, mp_time);
        // Print how many times faster multithread is. Will print speedup as "nan" if mp_time is 0
        printf("Multithread runs at %.1f times singlethread\n", (float) sp_time / (float) mp_time);
	}
    MPI_Finalize();
	return 0;
}
