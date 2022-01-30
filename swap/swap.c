#include <stdio.h>
#include <stdint.h>

#define dftlen 10

void main(void) {
    int16_t prachF[2*dftlen] = {0};


    for(int i = 0; i < dftlen; i++){
        ((int32_t *)&prachF[0])[i] = i;
    
        // if(i < 9)
            printf("%d ", ((int32_t *)&prachF[0])[i]);

    }


   // int k = 0;
    int32_t tmp;
    for(int i = 0; i < dftlen/2; i++){
        tmp = ((int32_t *)&prachF[0])[i];
        ((int32_t *)&prachF[0])[i] = ((int32_t *)&prachF[0])[dftlen/2+i];
        ((int32_t *)&prachF[0])[dftlen/2+i] = tmp;
    
    }

    printf("\n");


    for(int i = 0; i < dftlen; i++){
    
        // if(i > dftlen-5)
            printf("%d ", ((int32_t *)&prachF[0])[i]);

    }

    printf("\n");


}