#include <stdio.h>
#include <stdlib.h>

int main(){
    int n_x = 8, n_y = 8, n_z = 8;
    int *u = calloc(n_x * n_y * n_z, sizeof(int));

    for(int i = 0; i < n_x; i++){
        for(int j = 0; j < n_y; j++){
            for(int k = 0; k < n_z; k++){
                *(u + i * n_y * n_z + j * n_z + k) = i * n_y * n_z + j * n_z + k;
            }
        }
    }

    for(int i = 0; i < n_x; i++){
        for(int j = 0; j < n_y; j++){
            for(int k = 0; k < n_z; k++){
                printf("%6d ", *(u + i * n_y * n_z + j * n_z + k));
            }
            printf("\n");
        }
        printf("----\n");
    }

    free(u);

    return 0;
}
