/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    
    int abc;
    
} NR_COMMON;


typedef struct {

    NR_COMMON tmp;
    int **rxdata;
   
} NR_UE_COMMON; 


#define sum(a, b) a+b


void readFrame(NR_UE_COMMON *ue) {
    
    printf("In %s(), 25 sample = %d\n", __FUNCTION__, ue->rxdata[0][25]);
    
    // void *rxp[1];
    
    printf("123 %d\n", sum(1, 4));

    
    // for(int i = 0; i < 1; i++) {
    //     free(rxp[i]);
    // }    
    
    
    
}


int main()
{
    
    NR_UE_COMMON *UE;
    
    printf("%p\n%p\n", &UE->tmp.abc, UE);
    
    UE->tmp.abc = 2;
       
    printf("UE->tmp.abc %d\n", UE->tmp.abc);
    
    int nb_ant = 1;
    int nb_samples = 30720;
    
    UE->rxdata = (int **)malloc(sizeof(int*) * nb_ant);
    
    for(int i = 0; i < nb_ant; i++){
        UE->rxdata[i] = (int *)malloc(sizeof(int) * nb_samples);
    }


    for(int i = 0; i < 30720; i++){
        UE->rxdata[0][i] = i;
    }
    
    
    printf("sample 25 = %d\n", UE->rxdata[0][25]);
    
    readFrame(UE);

    
    
    for(int i = 0; i < nb_ant; i++){
        free(UE->rxdata[i]);
    }

    free(UE->rxdata);

    return 0;
}
