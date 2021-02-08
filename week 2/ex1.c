#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    FILE *fp;
    clock_t start_t, end_t;
    double total_t;
    int buffer_size = 4096;
    char *buffer = calloc(buffer_size, sizeof(char));

    start_t = clock();
    fp = fopen("ex1.bin", "r+b");
    fread(buffer, sizeof(char), buffer_size, fp);
    rewind(fp);
    fwrite(buffer, sizeof(char), buffer_size, fp);
    end_t = clock();
    fclose(fp);
    total_t = (double) (end_t - start_t) / CLOCKS_PER_SEC;
    printf("Time reading and writing 4kB (binary): %lfs\n", total_t);

    start_t = clock();
    fp = fopen("ex1.txt", "r+");
    fread(buffer, sizeof(char), buffer_size, fp);
    rewind(fp);
    fwrite(buffer, sizeof(char), buffer_size, fp);
    end_t = clock();
    fclose(fp);
    total_t = (double) (end_t - start_t) / CLOCKS_PER_SEC;
    printf("Time reading and writing 4kB (ASCII): %lfs\n", total_t);
}
