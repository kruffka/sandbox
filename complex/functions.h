#include <stdio.h>
#include <stdlib.h>

struct complex {
    int re;
    int im;
    void (*fun) (void);
};

typedef struct complex complex_t;

complex_t init();
complex_t* init2();
void free_struct(complex_t *a);

void msg(void);