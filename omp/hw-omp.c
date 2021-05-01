#include <stdio.h>
#include <omp.h>

int main(){
    #pragma omp parallel
    {
        int num_threads, thread_id;
        num_threads = omp_get_num_threads();
        thread_id = omp_get_thread_num();
        printf("Thread %d of %d\n", thread_id, num_threads);
    }
    return 0;
}
