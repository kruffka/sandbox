#include "functions.h"

struct point struct_init() {
    struct point p; // = (struct point *)malloc(sizeof(struct point));

    p.x = 2;
    p.y = 3;

    return p;
}

int pointSum(struct point a, point_t b) {

    return a.x + b.x;
}



// pointers

point_t* struct_init2() {
    return malloc(sizeof(point_t));
}

void struct_free(point_t *p) {
    free(p);
}