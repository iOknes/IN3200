#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void MPI_single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output) {
	int i, j, ii, jj;
	int my_rank, size, remainder, n_jobs, n_my_jobs;
	double temp;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	n_jobs = (M - K + 1);

	int *input_sendcounts = malloc(size * sizeof(int));
	int *input_displs = malloc(size * sizeof(int));
	remainder = n_jobs % size;
    for (i = 0; i < size; i++) {
        input_sendcounts[i] = (n_jobs / size) * N;
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
	
	int input_recvcount = input_sendcounts[my_rank];
	float **input_recvbuf = (float**) malloc(input_recvcount / N * sizeof(float*));
	input_recvbuf[0] = (float*) malloc(input_recvcount * sizeof(float));
	for (i = 1; i < input_recvcount / N; i++) {
		input_recvbuf[i] = &(input_recvbuf[0][i * N]);
	}

    n_my_jobs = ((input_recvcount / N) - K + 1);

	// Makes input a random pointer without any memory allocated to avoid NULL-pointer reading.
	if (my_rank > 0) {
		input = malloc(0);
	}

	// Allocate memory to input before calling this function!
	MPI_Scatterv(*input, input_sendcounts, input_displs, MPI_FLOAT, input_recvbuf[0], input_recvcount, MPI_FLOAT, 0, MPI_COMM_WORLD);

	float **output_sendbuf = (float**) malloc(n_my_jobs * sizeof(float*));
	output_sendbuf[0] = (float*) malloc(n_my_jobs * (N - K + 1) * sizeof(float));
	for (i = 1; i < n_my_jobs; i++) {
		output_sendbuf[i] = &(output_sendbuf[0][i * (N - K + 1)]);
	}

	for (i = 0; i < n_my_jobs; i++) {
    	for (j = 0; j <= N - K; j++) {
       		temp = 0.0;
			for (ii = 0; ii < K; ii++) {
				for (jj = 0; jj < K; jj++) {
					temp += input_recvbuf[i+ii][j+jj] * kernel[ii][jj];
				}
			}
			output_sendbuf[i][j] = temp;
		}
	}

	int *output_recvcounts, *output_displs;
	output_recvcounts = (int *) malloc(size * sizeof(int));
	output_displs = (int *) malloc(size * sizeof(int));

	for (i = 0; i < size; i++) {
		output_recvcounts[i] = ((input_sendcounts[i] / N) - K + 1) * (N - K + 1);
	}
	output_displs[0] = 0;
	for (i = 1; i < size; i++) {
		output_displs[i] = output_displs[i-1] + output_recvcounts[i-1];
	}

    /* Assign pointers to output to avoid segmentation fault when passing 
    output[0] to MPI_Gatherv on threads other than 0 */

	if (my_rank > 0) {
		output = malloc(0);
	}

	MPI_Gatherv(output_sendbuf[0], n_my_jobs * (N - K + 1), MPI_FLOAT, output[0], output_recvcounts, output_displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
}