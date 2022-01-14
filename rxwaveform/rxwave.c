#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void main(void){


    FILE *file_re = fopen("waveformRe.txt", "r");
    FILE *file_im = fopen("waveformIm.txt", "r");

    if(file_re == NULL || file_im == NULL){
        printf("error opening file\n");
        exit(0);
    }

    FILE *file_a = fopen("a.m", "w");

    fprintf(file_a, "rxdata = [");
    int16_t re, im;
    for(int i = 0; i < 1228800; i++){
        fscanf(file_re, "%hd", &re);
        fscanf(file_im, "%hd", &im);

        fprintf(file_a, "%hd + j*(%hd)\n", re, im);

    }
    fprintf(file_a, "];");


    fclose(file_a);
    fclose(file_re);
    fclose(file_im);

}