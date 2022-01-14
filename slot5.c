#include <stdio.h>


void main(void){


    int a = 307520;
    printf("symb | offset\n");
    for(int i = 0; i < 14; i++){
        printf("%d     | %d-%d\n", i, a, a+4096);
        if(i == 6) a += 4416;
        else a+=4384;
    }

}