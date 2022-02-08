#include "functions.h"

void main(void) {
    
    struct point my_point = struct_init();

    printf("my_point x: %d my_point y: %d\n", my_point.x, my_point.y);
    

    
    
    // pointers
    point_t *my_point2 = struct_init2();

    printf("my_point x: %d my_point y: %d\n", my_point2->x, my_point2->y);
    printf("x addr %p\ny addr %p\n", &my_point2->x, &my_point2->y);
    
    struct_free(my_point2);


    I_love_programming variable = 5;

    printf("I_love_programming var = %d\n", variable);

}