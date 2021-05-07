#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void MPI_single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output) {
	int i, j, ii, jj;
	int my_rank, size, remainder, n_jobs, n_my_jobs;
	double temp;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Calculate the total number of jobs to be done (number of rows in output)
	n_jobs = (M - K + 1);

	int *input_sendcounts = malloc(size * sizeof(int));
	int *input_displs = malloc(size * sizeof(int));

	// Calculate how many jobs must be distributed after naive distribution
	remainder = n_jobs % size;
    
	// Navily distribute the jobs (equal number for each thread)
	for (i = 0; i < size; i++) {
        input_sendcounts[i] = (n_jobs / size) * N;
    }
	/* Check if there are any remaining rows that have not been distributed. If
	there are, distribute them to the necessary number of threads so that lower 
	ranked threads get one extra each. */
	if (remainder != 0) {
		for (i = 0; i < remainder; i++) {
			input_sendcounts[i] += N;
		}
	}
	/* Calculate displacement in data for threads. The first thread will always
	be 0, and all following will be displaced by the number of values sent to
	all previous threads */
	input_displs[0] = 0;
	for (i = 1; i < size; i++) {
		input_displs[i] = input_displs[i-1] + input_sendcounts[i-1];
	}
	/* Send enough data to each thread so that they can do all their
	calculations. This is done after displacement is calculated so that the K-1 
	following rows that are erquired for each jobs calculations are also sent, 
	with overlap.
	
	The additional lines are sent before calculations as opposed to the 
	alternative of sending data between the threads as they need it. The method
	of sending all data beforehand means all threads can run independantly of 
	each other. This means that slowdown on one thread won't propagate to other
	threads. */
	for (i = 0; i < size; i++) {
		input_sendcounts[i] += (K - 1) * N;
	}
	
	// Each thread stores how many elements it's about to receive
	int input_recvcount = input_sendcounts[my_rank];

	/* Each thread allocates a 2D array for the data it is about to recieve,
	with which row of data being recieved as first index. Outer pointer array
	has size input_recvcount / N as this is the number of rows being recieved. 
	It will always be divisible without remainder based on how it was calculated
	*/
	float **input_recvbuf = (float**) malloc(input_recvcount / N * sizeof(float*));
	input_recvbuf[0] = (float*) malloc(input_recvcount * sizeof(float));
	for (i = 1; i < input_recvcount / N; i++) {
		input_recvbuf[i] = &(input_recvbuf[0][i * N]);
	}

	// Each thread stores the number of jobs it has to do
    n_my_jobs = ((input_recvcount / N) - K + 1);

	/* Input is only set on thread 0. To avoid segmentation fault by reading 
	from NULL[0] in MPI_Scatterv each non-zero thread assigns 0 bytes and stores
	the pointer to input. This avoids the error without allocating unnecessary 
	memory */
	if (my_rank > 0) {
		input = malloc(0);
	}

	MPI_Scatterv(*input, input_sendcounts, input_displs, MPI_FLOAT, input_recvbuf[0], input_recvcount, MPI_FLOAT, 0, MPI_COMM_WORLD);

	/* Allocate an array on each thread to store data that is to be sent back to
	thread 0 in MPI_Gatherv */
	float **output_sendbuf = (float**) malloc(n_my_jobs * sizeof(float*));
	output_sendbuf[0] = (float*) malloc(n_my_jobs * (N - K + 1) * sizeof(float));
	for (i = 1; i < n_my_jobs; i++) {
		output_sendbuf[i] = &(output_sendbuf[0][i * (N - K + 1)]);
	}

	/* Calculate the convolution for each threads data and store in 
	output_sendbuf */
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

	/* Allocate arrays for counts and displacements for returning data to
	thread 0 */
	int *output_recvcounts, *output_displs;
	output_recvcounts = (int *) malloc(size * sizeof(int));
	output_displs = (int *) malloc(size * sizeof(int));

	/* Filling output_recvcounts and output_displs. Counts is just numer of jobs
	each thread has, and the displacement is just the cumulative of the 
	counts. */
	for (i = 0; i < size; i++) {
		output_recvcounts[i] = ((input_sendcounts[i] / N) - K + 1) * (N - K + 1);
	}
	output_displs[0] = 0;
	for (i = 1; i < size; i++) {
		output_displs[i] = output_displs[i-1] + output_recvcounts[i-1];
	}

    /* Assign pointers to output to avoid segmentation fault when passing 
    output[0] to MPI_Gatherv on threads other than 0. Same as was done for
	MPI_Scatterv earlier. */

	if (my_rank > 0) {
		output = malloc(0);
	}

	/* The data from each thread is gathered back to thread 0 who stores it at
	output[0]. This means that thread 0 returns the reassembled data via this 
	pointer, and it is accessable to the calling function. */
	MPI_Gatherv(output_sendbuf[0], n_my_jobs * (N - K + 1), MPI_FLOAT, output[0], output_recvcounts, output_displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
}