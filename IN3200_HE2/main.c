#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output);
void MPI_single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output);

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
		kernel = (float **) malloc(K * sizeof(float));
        kernel[0] = (float *) calloc(K * K, sizeof(float));
        for (i = 1; i < K; i++) {
            kernel[i] = &(kernel[0][i * K]);
        }
	}
	// process 0 broadcasts the content of kernel to all the other processes
	// ...
	MPI_Bcast(kernel[0], K * K, MPI_FLOAT, 0, MPI_COMM_WORLD);
	//printf("Kernel has been broadcast on thread %d...\n", my_rank);
	// parallel computation of a single-layer convolution
	MPI_single_layer_convolution (M, N, input, K, kernel, output);
	printf("MPI_single_layer_convolution has been completed on thread %d\n", my_rank);
	/*if (my_rank == 0) {
		float **singlethread_output;
		int comparison = 1;
		// For example, compare the content of array ’output’ with that is
		// produced by the sequential function single_layer_convolution
		// ... 
		singlethread_output = (float **) malloc((M-K+1) * sizeof(float *));
        singlethread_output[0] = (float *) calloc((M-K+1) * (N-K+1), sizeof(float));
        for (i = 1; i <= M-K; i++) {
            singlethread_output[i] = &(singlethread_output[0][i * (N-K+1)]);
        }
		
		single_layer_convolution(M, N, input, K, kernel, singlethread_output);

		for (i = 0; i < M; i++) {
			for (j = 0; j < N; j++) {
				comparison *= singlethread_output[i][j] == output[i][j];
			}
		}
	}*/
    MPI_Finalize();
	return 0;
}

void MPI_single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output) {
	int i, j, ii, jj;
	int my_rank, size, remainder, n_jobs;
	double temp;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	//printf("MPI_single_layer_convolution has been entered on thread %d\n", my_rank);
	//printf("input: %p on thread %d\n", input, my_rank);

	n_jobs = (M - K + 1);

	int *input_sendcounts = malloc(size * sizeof(int));
	int *input_displs = malloc(size * sizeof(int));
	remainder = n_jobs % size;
    for (i = 0; i < size; i++) {
        input_sendcounts[i] = (n_jobs / size) * N;
        //printf("input_sendcounts[%d] = %d on thread %d\n",i, input_sendcounts[i], my_rank);
    }
	if (remainder != 0) {
		for (i = 0; i < remainder; i++) {
			input_sendcounts[i] += N;
		}
	}
	input_displs[0] = 0;
	for (i = 1; i < size; i++) {
		input_displs[i] = input_displs[i-1] + input_sendcounts[i-1];
	}
	// Send enough data to each thread so that they can do all their calculations
	/* This is done after displacement is calculated so that the K-1 next rows
	that are erquired for the calculations are also sent, but with overlap.*/
	for (i = 0; i < size; i++) {
		input_sendcounts[i] += (K - 1) * N;
	}
	
	// printing for debuging
	/*printf("Printing input_sendcounts on thread %d:\n", my_rank);
	for (i = 0; i < size; i++) {
		printf("%d ", input_sendcounts[i]);
	} putchar('\n');
	
	printf("Printing input_displs on thread %d:\n", my_rank);
	for (i = 0; i < size; i++) {
		printf("%d ", input_displs[i]);
	} putchar('\n');*/

	//printf("Send counts and displacements have been calculated on thread %d...\n", my_rank);

	int input_recvcount = input_sendcounts[my_rank];
	printf("Input receive count has been defined on thread %d...\n", my_rank);
	float **input_recvbuf = (float**) malloc(input_recvcount / N * sizeof(float*));
	input_recvbuf[0] = (float*) malloc(input_recvcount * sizeof(float));
	for (i = 1; i < input_recvcount / N; i++) {
		input_recvbuf[i] = &(input_recvbuf[0][i * N]);
	}
	
	//printf("Input receive buffer has been allocated on thread %d...\n", my_rank);

	//printf("Testing input[0] pointer on thread %d: %p\n", my_rank, input[0]);
	//printf("Testing input_sendcounts pointer on thread %d: %p\n", my_rank, input_sendcounts);
	//printf("Testing input_displs pointer on thread %d: %p\n", my_rank, input_displs);
	//printf("Testing input_recvbuf pointer on thread %d: %p\n", my_rank, input_recvbuf);
	
	//input = (float **) malloc(sendcounts[]);

	// Makes input a random pointer without any memory allocated to avoid NULL-pointer reading.
	if (my_rank > 0) {
		input = malloc(0);
	}

	// Allocate memory to input before calling this function!
	MPI_Scatterv(*input, input_sendcounts, input_displs, MPI_FLOAT, input_recvbuf[0], input_recvcount, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if (my_rank >= 0) {
		printf("Input of size %d has been scattered on thread %d...\n",input_sendcounts[my_rank], my_rank);
	}
	/*for (i = 0; i < input_recvcount; i++) {
		printf("%4.2f ", input_recvbuf[0][i]);
	} putchar('\n');*/

	float **output_sendbuf = (float**) malloc(((input_recvcount / N) - K + 1) * sizeof(float*));
	output_sendbuf[0] = (float*) malloc(((input_recvcount / N) - K + 1) * (N - K + 1) * sizeof(float));
	for (i = 1; i < ((input_recvcount / N) - K + 1); i++) {
		output_sendbuf[i] = &(output_sendbuf[0][i * (N - K + 1)]);
	}

	if (my_rank >= 0) {
		printf("Output_sendbuf has been allocated on thread %d...\n", my_rank);
	}

	printf("Checking kernel on thread %d:\n", my_rank);
	for (i = 0; i < K * K; i++) {
		printf("%4.2f ", kernel[0][i]);
	} putchar('\n');

	printf("Calc. loop boundry on thread %d: %d\n", my_rank, (input_recvcount / N) - K + 1);

	for (i = 0; i < (input_recvcount / N) - K + 1; i++) {
    	for (j = 0; j <= N - K; j++) {
       		temp = 0.0;
			printf("Calculating [%d][%d] on thread %d...\n", i, j, my_rank);
			for (ii = 0; ii < K; ii++) {
				for (jj = 0; jj < K; jj++) {
					printf("Calculating [%d][%d] in [%d][%d] on thread %d...\n", ii, jj, i, j, my_rank);
					// Seg faults at ii=2, jj=0, i=0, j=0 on thread 2 and 3
					temp += input_recvbuf[i+ii][j+jj] * kernel[ii][jj];
				}
			}
			output_sendbuf[i][j] = temp;
		}
	}
	if (my_rank > 0) {
		printf("Output_sendbuf has been filled on thread %d...\n", my_rank);
	}

	int *output_recvcounts, *output_displs;
	// Allocate and assign values to above variables
	output_recvcounts = (int *) malloc(size * sizeof(int));
	output_displs = (int *) malloc(size * sizeof(int));
	
	if (my_rank > 0) {
		printf("Output_recvcounts and _displs has been allocated on thread %d...\n", my_rank);
	}

	for (i = 0; i < size; i++) {
		output_recvcounts[i] = ((input_sendcounts[i] / N) - K + 1) * (N - K + 1);
	}
	output_displs[0] = 0;
	for (i = 1; i < size; i++) {
		output_displs[i] = output_displs[i-1] + output_recvcounts[i-1];
	}
	
	if (my_rank > 0) {
		printf("Output_recvcounts and _displs has been filled on thread %d...\n", my_rank);
	}
	if (my_rank >= 0) {
		printf("Printing output_recvcounts on thread %d:\n", my_rank);
		for (i = 0; i < size; i++) {
			printf("%d ", output_recvcounts[i]);
		} putchar('\n');
		
		printf("Printing output_displs on thread %d:\n", my_rank);
		for (i = 0; i < size; i++) {
			printf("%d ", output_displs[i]);
		} putchar('\n');
		
		printf("Printing output_sendbuf on thread %d (should be length %d):\n", my_rank, ((input_recvcount / N) - K + 1) * (N - K + 1));
		for (i = output_displs[my_rank]; i < output_displs[my_rank] + ((input_recvcount / N) - K + 1) * (N - K + 1); i++) {
			printf("%6.2f ", output_sendbuf[0][i]);
		} putchar('\n');
	}

	if (my_rank > 0) {
		output = malloc(0);
	}

	printf("Gathering on thread %d...\n", my_rank);
	MPI_Gatherv(output_sendbuf[0], ((input_recvcount / N) - K + 1) * (N - K + 1), MPI_FLOAT, *output, output_recvcounts, output_displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
	printf("Output has been gathered on thread %d...\n", my_rank);
	//MPI_Gatherv(&temp_sum, 1, MPI_INT, sums, recvcounts, recvdispls, MPI_INT, 0, MPI_COMM_WORLD);
}

void single_layer_convolution (int M, int N, float **input, int K, float **kernel, float **output) {
  	int i,j,ii,jj;
  	double temp;
  	for (i=0; i<=M-K; i++)
    	for (j=0; j<=N-K; j++) {
       		temp = 0.0;
			for (ii=0; ii<K; ii++)
				for (jj=0; jj<K; jj++)
					temp += input[i+ii][j+jj]*kernel[ii][jj];
			output[i][j] = temp;
		}
}
