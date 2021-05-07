#include <stdio.h>
#include <stdlib.h>

int main() {
    void *a = malloc(0);

    printf("*a: %p\n", a);

    return 0;
}