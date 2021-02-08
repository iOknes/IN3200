#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

double pow100(double x);
double pow100_fast(double x);
double pow100_faster(double x);

int main(){
    double x, p, cpu_time_used;
    clock_t start, clocks;

    srand(time(0));
    x = rand() / (double) RAND_MAX;
    printf("%f\n", x);
    
    //Math.h power function
    start = clock();
    p = pow(x,100);
    clocks = clock() - start;
    cpu_time_used = ((double) clocks) / CLOCKS_PER_SEC;
    printf("Math.h pow\n");
    printf("P: %4.8lf, clocks: %4.2lf, time: %lfms\n", p, (double) clocks, cpu_time_used * 1000);
    
    //For-loop
    start = clock();
    p = pow100(x);
    clocks = clock() - start;
    cpu_time_used = ((double) clocks) / CLOCKS_PER_SEC;
    printf("pow100\n");
    printf("P: %4.8lf, clocks: %4.2lf, time: %lfms\n", p, (double) clocks, cpu_time_used * 1000);

    //Exponentiation by squaring (with loops)
    start = clock();
    p = pow100_fast(x);
    clocks = clock() - start;
    cpu_time_used = ((double) clocks) / CLOCKS_PER_SEC;
    printf("pow100_fast\n");
    printf("P: %4.8lf, clocks: %4.2lf, time: %lfms\n", p, (double) clocks, cpu_time_used * 1000);
    
    //Exponentiation by squaring
    start = clock();
    p = pow100_faster(x);
    clocks = clock() - start;
    cpu_time_used = ((double) clocks) / CLOCKS_PER_SEC;
    printf("pow100_faster\n");
    printf("P: %4.8lf, clocks: %4.2lf, time: %lfms\n", p, (double) clocks, cpu_time_used * 1000);
}

double pow100(double x){
    int y = 100;
    double p = x;
    for(int i = 0; i < y-1; i++){
        p *= x;
    }
    return p;
}

double pow100_fast(double x){
    for(int i = 0; i < 2; i++){
        x *= x;
    }
    double x_ = x;
    for(int i = 0; i < 3; i++){
        x *= x;
    }
    double x__ = x;
    x *= x;
    return x * x_ * x__;
}

double pow100_faster(double x){
    x *= x;
    x *= x;
    double x4 = x;
    x *= x;
    x *= x;
    x *= x;
    double x32 = x;
    x *= x;
    return x * x4 * x32;
}
