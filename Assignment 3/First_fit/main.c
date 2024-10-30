#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {
    demonstration();
    //varied_demonstration();

    printf("Heap addresses:\n");
    extern void* global_base;
    printf("Start of the heap: %p\n", global_base);
    printf("End of the heap: %p\n", sbrk(0));

    printf("Checking for memory leaks\n");
    check_leaks();
    return 0;
}