#include <immintrin.h>
#include <stdio.h>

void printVeci16(__m256i vec)
{
    unsigned short int tempi16[16];
    _mm256_store_si256((__m256i *)&tempi16[0], vec);
    printf("[0]=%2u, [1]=%2u, [2]=%2u, [3]=%2u, [4]=%2u, [5]=%2u, [6]=%2u, [7]=%2u,... [8]=%2u, [9]=%2u, [10]=%2u, [11]=%2u, [12]=%2u, [13]=%2u, [14]=%2u, [15]=%2u \n\n",
     tempi16[0],tempi16[1],tempi16[2],tempi16[3],tempi16[4],tempi16[5],tempi16[6], tempi16[7],tempi16[8],tempi16[9],tempi16[10],tempi16[11],tempi16[12],tempi16[13],tempi16[14],tempi16[15] ) ;

}

int main()
{
    short x[16], y[16];
    // __m256i a,  b, temp_a, temp_b;
    // __m256i mask = _mm256_set_epi8( 0,0, 0x80,0x80, 0,0, 0,0, 0x80,0x80, 0,0, 0,0       ,0x80,0x80, 0,0, 0,0, 0x80,0x80, 0,0, 0,0, 0x80,0x80, 0,0, 0,0);
    printf("123\n");
    for(int i=0; i<16; i++) {
        x[i] = i+1; 
        y[i] = i+17;
    } // make some values for vectors

    // a =  _mm256_loadu_si256((__m256i*)x); 
    // b = _mm256_loadu_si256((__m256i*)y);

    // printf(" original    a : "); printVeci16(a); 
    // printf(" original    b : "); printVeci16(b); 

    // temp_a = _mm256_blendv_epi8 (a, b, mask);
    // temp_b = _mm256_blendv_epi8 (b, a, mask);

    // a = temp_a;
    // b = temp_b;

    // printf("swap         a : ");printVeci16(a); 
    // printf("swap         b : ");printVeci16(b); 

    return 0;


}