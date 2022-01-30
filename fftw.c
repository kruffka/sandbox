/* Start reading here */

#include <fftw3.h>

#define NUM_POINTS 4096   


/* Never mind this bit */

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define REAL 0
#define IMAG 1
int symb = 0;
char filename[40];

void acquire_from_somewhere(fftw_complex* signal) {


    sprintf(filename, "dft/rxdata_symb%d.m", symb);
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("fopen_r\n");
        exit(0);
    }

    int16_t re, im;
    fscanf(file, "rxdata = [");
    for(int i = 0; i < NUM_POINTS; i++){
        fscanf(file, "%hd + j*(%hd)\n", &re, &im);
        signal[i][REAL] = re / 32767.0f;
        signal[i][IMAG] = im / 32767.0f;
    }
    fscanf(file, "];");

    fclose(file);
    
    
    // for(int i = 0; i < 2; i++){
    //     printf("%f %f %d %d\n", signal[i][REAL], signal[i][IMAG], (int16_t)(signal[i][REAL]*32767 + 1), (int16_t)(signal[i][IMAG]*32767 + 1));
    // }
}

void do_something_with(fftw_complex* result) {

        sprintf(filename, "dft_fftw/rxdataF_symb%d_fftw.m", symb);
        FILE *file = fopen(filename, "w");
        if(file == NULL){
            printf("fopen_w\n");
            exit(0);
        }
        fprintf(file, "rxdataF_fftw = [");
        for(int i = 0; i < NUM_POINTS; i++){
            fprintf(file, "%hd + j*(%hd)\n", (int16_t)(result[i][REAL]*32767 + 1), (int16_t)(result[i][IMAG]*32767 + 1));
        }
        fprintf(file, "];");


        fclose(file);


        for(int i = 0; i < 5; i++){
            printf("%g + %gi\n", result[i][REAL], result[i][IMAG]);

        }
}


/* Resume reading here */

int main() {
    fftw_complex signal[NUM_POINTS];
    fftw_complex result[NUM_POINTS];

    fftw_plan plan = fftw_plan_dft_1d(NUM_POINTS,
                                      signal,
                                      result,
                                      FFTW_FORWARD,
                                      FFTW_ESTIMATE);

    for(symb = 0; symb < 14; symb++){
        acquire_from_somewhere(signal);
        fftw_execute(plan);
        do_something_with(result);
    }
    


    fftw_destroy_plan(plan);

    printf("done\n");

    return 0;
}