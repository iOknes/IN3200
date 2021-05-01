#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(){
    int N = 4194304;
    int i, s, N_per_thread;
    int *a;
    a = malloc(N * sizeof(int));
    clock_t start, end;
    float t;

    for (i = 0; i < N; i++) {
        a[i] = i;
    }
    
    N_per_thread = 

    start = clock();
    s = 0;
    #pragma omp for firstprivate(s)
    for (i = 0; i < N; i++) {
        s += a[i];
    }
    end = clock();
    t = (float) (end - start);
    t /= CLOCKS_PER_SEC;

    printf("Sum: %d\nTime: %.3f\n", s, t);
}
