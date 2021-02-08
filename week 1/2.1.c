#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
    FILE *fp;
    char c;
    int n;

    fp = fopen("weather.txt", "r");
    if(fp == NULL){
        exit(0);
    }

    /*c = fgetc(fp);
    while(c != EOF){
        if(c == '\n'){
            n++;
        }
        c = fgetc(fp);
    }
    rewind(fp);*/

    char time[n][5];
    float temp[n];
    for(int i = 0; i < n; i++){
        fscanf(fp, "%s %f\n", time[i], &temp[i]);
    }
    float min = temp[0], max = temp[0];
    for(int i = 0; i < n; i++){
        if(min > temp[i]){
            min = temp[i];
        }
    }
    for(int i = 0; i < n; i++){
        if(max < temp[i]){
            max = temp[i];
        }
    }
    //Calculate average
    float avg, s;
    for(int i = 0; i < n; i++){
        s += temp[i];
    }
    avg = s / n;
    //Calculate std. div.
    float sigma;
    for(int i = 0; i < n; i++){
        sigma += pow(temp[i] - avg, 2);
    }
    sigma /= n;
    sigma = sqrt(sigma);
    printf("Min: %.2f\nMax: %.2f\nAvg: %.2f\nStd. div: %.2f\n", min, max, avg, sigma);

    return 0;
}
