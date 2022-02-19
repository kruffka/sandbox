#include <stdio.h>
#include <stdlib.h>

typedef struct complex {
    int re;
    int im;
} complex_t;

// typedef struct complex complex_t;

complex_t init();
complex_t* init2();
void free_struct(complex_t *a);
