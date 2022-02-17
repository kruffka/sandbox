#include "functions.h"

typedef enum {
    n = 99999
} array_size;

void msg(void) {
    printf("hello, world\n");
}


void main() {

    int a[n];
    struct complex p = init();
    

    p.fun = msg;
    p.fun();

    printf("re %d im %d\n", p.re, p.im);

    complex_t *pointer = init2();

    (*pointer).re = 2;
    pointer->re = 2;

    free_struct(pointer);


}