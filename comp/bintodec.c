#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void main(int argc, char **argv){ //int argc, char **argv

    if (argc == 1) {
        printf("Nothing in args\n");
        exit(0);
    }

    uint16_t a, b;
    int32_t c;

    if(atoi(argv[1]) == 0) {
        printf("Input %%hd+j*%%hd:\n");
        scanf("%hd+j*%hd", &a, &b);
        c = (b << 16) | a;
    } else {
        printf("Input %%d\n");
        scanf("%d", &c);
        b = c >> 16;
        a = c << 16 >> 16;
    }
    

    printf("%hd+j*%hd %d\n", a, b, c);

}