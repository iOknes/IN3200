#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int main(){
    clock_t start_t;
    double total_t;
    int n = 1024, i;
    int *a = calloc(n, sizeof(int));
    int *b = calloc(n, sizeof(int));

    start_t = clock();
    memcpy(b, a, n * sizeof(int));
    total_t = (double) (clock() - start_t) / CLOCKS_PER_SEC;
    printf("Time elapsed copying (memcpy()): %lf\n", total_t);

    start_t = clock();
    for (i = 0; i < n; i++) b[i] = a[i];
    total_t = (double) (clock() - start_t) / CLOCKS_PER_SEC;
    printf("Time elapsed copying (for-loop): %lf\n", total_t);

    return 0;
}
