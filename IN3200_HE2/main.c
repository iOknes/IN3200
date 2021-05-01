#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void MPI_single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output);

int main (int nargs, char **args){
  int M=0, N=0, K=0, my_rank;
  float **input=NULL, **output=NULL, **kernel=NULL;
  MPI_Init(&nargs, &args);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  if (my_rank==0) {
    // read from command line the values of M, N, and K
    if (nargs >= 4){
      M = atoi(args[1]);
      N = atoi(args[2]);
      K = atoi(args[3]);
    } else {
      printf("Enter value for M:\n");
      scanf("%d", &M);
      printf("Enter value for N:\n");
      scanf("%d", &N);
      printf("Enter value for K:\n");
      scanf("%d", &K);
    }
    printf("M: %d, N: %d, K: %d\n", M, N, K);
    MPI_Finalize();
    return 0;
    // allocate 2D array ’input’ with M rows and N columns
    input = calloc(M * N, sizeof(float));
    // allocate 2D array ’output’ with M-K+1 rows and N-K+1 columns
    // allocate the convolutional kernel with K rows and K columns
    // fill 2D array ’input’ with some values
    // fill kernel with some values
    // ....
  }
  // process 0 broadcasts values of M, N, K to all the other processes
  // ...
  if (my_rank>0) {
    // allocated the convolutional kernel with K rows and K columns
    // ...
  }
  // process 0 broadcasts the content of kernel to all the other processes
  // ...
  // parallel computation of a single-layer convolution
  MPI_single_layer_convolution(M, N, input, K, kernel, output);
  if (my_rank==0) {
    // For example, compare the content of array ’output’ with that is
    // produced by the sequential function single_layer_convolution
    // ... 
  }
  MPI_Finalize();
  return 0;
}

void single_layer_convolution(int M, int N, float **input, int K, float **kernel, float **output){
  int i, j, ii, jj;
  double temp;
  for (i=0; i<=M-K; i++)
    for (j=0; j<=N-K; j++) {
       temp = 0.0;
        for (ii=0; ii<K; ii++)
            for (jj=0; jj<K; jj++)
                temp += input[i + ii][j + jj] * kernel[ii][jj];
        output[i][j] = temp;
    } 
}

void MPI_single_layer_convolution (int M, int N, float **input, int K, float **kernel, float **output) {
    int i, j, ii, jj;
    double temp;

    //MPI_Gatherv();
}