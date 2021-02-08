#include <stdio.h>
#include <stdlib.h>

int main(){
    int m, n;
    m = 8;
    n = 8;
    int mat[m][n];
    int mat_inv[m][n];
    int mat_com[m][n];

    //Generate matrix the default way
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            mat[i][j] = i * j;
        }
    }

    //Generate matrix with columns in outer loop
    for(int j = 0; j < n; j++){
        for(int i = 0; i < m; i++){
            printf("%d, %d\n", i, j);
            mat[i][j] = i * j;
        }
    }

    //Generate bool-matrix
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            mat_com[i][j] = (mat[i][j] == mat_inv[i][j]);
        }
    }

    //Print matrixes
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%5d ", mat[i][j]);
        }
        printf("\n");
    }
    putchar('\n');
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%5d ", mat_inv[i][j]);
        }
        printf("\n");
    }
    putchar('\n');
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%5d ", mat_com[i][j]);
        }
        printf("\n");
    }
    return 0;
}
