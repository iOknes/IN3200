#include <stdio.h>
#include <math.h>

int main(){
    double s = 0;
    for(int i = 0; i < 101; i++){
        s += pow(-1, i) * (1 / pow(2, 2*i));
    }
    printf("%lf\n", s);
    return 0;
}
