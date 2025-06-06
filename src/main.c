#include <stdio.h>
#include "heap.h"

int main(void) {
    char* foo = alloc(10);
    for (int i = 0; i < 10; i++) {
        foo[i] = 'A' + i;
    }
    printf("%s\n", foo);
}