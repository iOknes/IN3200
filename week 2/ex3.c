#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define idx(i, j, k, nx, ny) nx * ny * i + ny * j + k

int main(){
    int n, n_x, n_y, n_z;
    double ***u, ***v;
    clock_t start;

    n = 1024;
    n_x = n_y = n_z = 8;

    //Create array with indexes for u
    u = (double ***)malloc(n_z * sizeof(double **));
    u[0] = (double **)malloc(n_z * n_y * sizeof(double *));
    u[0][0] = (double *)malloc(n_z * n_y * n_x * sizeof(double));

    for (int i = 1; i < n_z; i ++) {
        u[i] = &u[0][n_y * i];
    }

    for (int j = 1; j < n_z * n_y; j++) {
        u[0][j] = &u[0][0][j * n_x];
    }

    //Create array with indexes for v
    v = (double ***)malloc(n_z * sizeof(double **));
    v[0] = (double **)malloc(n_z * n_y * sizeof(double *));
    v[0][0] = (double *)malloc(n_z * n_y * n_x * sizeof(double));

    for (int i = 1; i < n_z; i ++) {
        v[i] = &v[0][n_y * i];
    }

    for (int j = 1; j < n_z * n_y; j ++) {
        v[0][j] = &v[0][0][j * n_x];
    }

    //Populate v
    for (int i = 0; i < n_z; i++){
        for (int j = 0; j < n_y; j++){
            for (int k = 0; k < n_x; k++){
                v[i][j][k] = (double) 2.0 - sin(((double) M_PI) * i * j * k / (double)((n_x - 1) * (n_y -1) * (n_z - 1)));
            }
        }
    }

    //Populate u
    for (int i = 0; i < n_z; i++){
        for (int j = 0; j < n_y; j++){
            for (int k = 0; k < n_x; k++){
                u[i][j][k] = (double) 0;
            }
        }
    }

    //Main iteration loop
    for (int t = 0; t < n; t++){
        //Loop over the array
        for (int i = 1; i < n_z - 2; i++){
            for (int j = 1; j < n_y - 2; j++){
                for (int k = 1; k < n_x - 2; k++){
                    u[i][j][k] = v[i][j][k] + (v[i-1][j][k] + v[i][j-1][k] + v[i][j][k-1] - 6 * v[i][j][k] + v[i+1][j][k] + v[i][j+1][k] + v[i][j][k+1]);
                }
            }
        }
        //Copy u to v
        for (int i = 0; i < n_z; i++){
            for (int j = 0; j < n_y; j++){
                for (int k = 0; k < n_x; k++){
                    v[i][j][k] = u[i][j][k];
                }
            }
        }
    }
    
    //Deallocate arrays
    free(u);
    free(v);

    return 0;
}
