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

	printf("Checking kernel pointer integrity on thread %d:\n", my_rank);
	for (i = 0; i < K; i++) {
		printf("%p ", kernel[i]);
	} putchar('\n');

	printf("Calc. loop boundry on thread %d: %d\n", my_rank, (input_recvcount / N) - K + 1);

	for (i = 0; i < (input_recvcount / N) - K + 1; i++) {
    	for (j = 0; j <= N - K; j++) {
       		temp = 0.0;
			//printf("Calculating [%d][%d] on thread %d...\n", i, j, my_rank);
			for (ii = 0; ii < K; ii++) {
				for (jj = 0; jj < K; jj++) {
					// Seg faults at ii=2, jj=0, i=0, j=0 on thread 2 or 3 in accessing kernel
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
	MPI_Gatherv(output_sendbuf[0], ((input_recvcount / N) - K + 1) * (N - K + 1), MPI_FLOAT, output[0], output_recvcounts, output_displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
	printf("Output has been gathered on thread %d...\n", my_rank);
	//MPI_Gatherv(&temp_sum, 1, MPI_INT, sums, recvcounts, recvdispls, MPI_INT, 0, MPI_COMM_WORLD);
}