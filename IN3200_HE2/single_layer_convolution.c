#include <mpi.h>

void single_layer_convolution (int M, int N, float **input, int K, float **kernel, float **output){
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
