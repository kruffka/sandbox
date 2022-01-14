#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void read_rxdata(void *data, char *filename, char *arr){
 
    FILE *file;
    file = fopen(filename, "r");
    printf("filename = %s\n", filename);
 
    if(file == NULL){
      printf("Error reading file %s in %s\n", filename, __FUNCTION__);
      exit(0);
    }
    
    printf("array name: %s\n", arr);
    char rxdata[40];
    sprintf(rxdata, "%s = [", arr);
    fscanf(file, rxdata);

    // int32_t data = 0;
    int i = 0;
    for(; i < 1228800*2; i += 2){
        // printf("i = %d\n", i);
        fscanf(file, "%hd + j*(%hd)\n", &((short *)data)[i], &((short *)data)[i + 1]);


        ((short *)data)[i] /= 4;
        ((short *)data)[i + 1] /= 4;   
        // printf("i = %d data[0[ = %hd data[1[ = %hd\n", i/2, ((short *)data)[i], ((short *)data)[i+1]);
        // if(i == 4096*2 + 20) {
        //   exit(0);
        // }
        // sleep(1);
 
        // i += 2;
         
    }
    fscanf(file, "];");
    // for(int i = 0; i < 32; i += 2)
    printf("i = %d data[0[ = %hd data[1[ = %hd\n", i/2, ((short *)data)[0], ((short *)data)[1]);
 
 
 
    fclose(file);
 
}

void write_rxdata(void *data, char *filename, char *arr){
 
    FILE *file;
    file = fopen(filename, "w");
    printf("filename = %s\n", filename);
 
    if(file == NULL){
      printf("Error with file %s in %s\n", filename, __FUNCTION__);
      exit(0);
    }
    
    printf("array name: %s\n", arr);
    char rxdata[40];
    sprintf(rxdata, "%s = [", arr);
    fprintf(file, rxdata);

    // int32_t data = 0;
    int i = 0;
     for(; i < 1228800*2; i += 2){
        // printf("i = %d\n", i);
        fprintf(file, "%hd + j*(%hd)\n", ((short *)data)[i], ((short *)data)[i + 1]);
 
        // printf("i = %hd data[0[ = %hd data[1[ = %d\n", i/2, ((short *)data)[i], ((short *)data)[i+1]);
        // if(i == 4096*2 + 20) {
        //   exit(0);
        // }
        // sleep(1);
 
        // i += 2;
         
    }
    fprintf(file, "];");
    // for(int i = 0; i < 32; i += 2)
    printf("i = %d data[0[ = %d data[1[ = %d\n", i/2, ((short *)data)[0], ((short *)data)[1]);
 
 
 
    fclose(file);
 
}


void main(void){

    int32_t rxdata[1][1228800];
    char filename[40];
    char arrname[40];

    // for(int i = 0; i < 14; i++){
        // sprintf(filename, "dft/rxdata_symb%d.m", i);
        sprintf(filename, "dft/a.m");
        sprintf(arrname, "rxdata");
        read_rxdata(&rxdata[0][0], filename, "rxdata");

        sprintf(filename, "amp_div4/ad4.m");
        write_rxdata(&rxdata[0][0], filename, arrname);
    // }




}