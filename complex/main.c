#include "functions.h"

void main() {

    struct complex p = init();

    printf("re %d im %d\n", p.re, p.im);

    complex_t *pointer = init2();

    (*pointer).re = 2;
    pointer->re = 2;
    
    free_struct(pointer);


}