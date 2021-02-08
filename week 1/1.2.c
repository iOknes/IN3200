#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(0));

    char* input = malloc(80);
    printf("Enter array length:\n");
    gets(input);
    int n = atoi(input);
    double arr[n];
    for(int i = 0; i < n; i++){
        arr[i] = rand();
    }
    unsigned long min, max;
    min = arr[0];
    max = arr[0];
    for(int i = 0; i < n; i++){
        if(min > arr[i]){
            min = arr[i];
        }
    }
    for(int i = 0; i < n; i++){
        if(max < arr[i]){
            max = arr[i];
        }
    }
    for(int i = 0; i < n; i++){
        printf("%lf\n", arr[i]);
    }
    printf("Min: %lu, Max: %lu\n", min, max);
    return 0;
}
