#include <stdio.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <stdint.h>

#define print_shorts(s,x) printf("%s = [%d+j*%d, %d+j*%d, %d+j*%d, %d+j*%d]\n",s,(x)[0],(x)[1],(x)[2],(x)[3],(x)[4],(x)[5],(x)[6],(x)[7])
#define print_ints(s,x) printf("%s = %d %d %d %d\n",s,(x)[0],(x)[1],(x)[2],(x)[3])
#define simd_q15_t __m128i
#define set1_int16(a) _mm_set1_epi16(a)
#define mulhi_s1_int16(a,b) _mm_slli_epi16(_mm_mulhi_epi16(a,b),2)

const short conjugate[8]__attribute__((aligned(16))) = {-1,1,-1,1,-1,1,-1,1};
const short conjugate2[8]__attribute__((aligned(16))) = {1,-1,1,-1,1,-1,1,-1};


void multadd_real_vector_complex_scalar(int16_t *x,      // filter
                                        int16_t *alpha,  // ch input
                                        int16_t *y,      // dl_ch output
                                        uint32_t N)      // 8
{

  uint32_t i;

  // do 8 multiplications at a time
  __m128i alpha_r_128,alpha_i_128,yr,yi,*x_128=(__m128i *)x,*y_128=(__m128i *)y;
  int j;

   // printf("alpha = %d,%d\n",alpha[0],alpha[1]);
   alpha_r_128 = _mm_set1_epi16(alpha[0]);
   alpha_i_128 = _mm_set1_epi16(alpha[1]);

   j=0;

   for (i=0; i<N>>3; i++) {

      yr     =  _mm_slli_epi16(_mm_mulhi_epi16(alpha_r_128,x_128[i]),2);
      yi     =  _mm_slli_epi16(_mm_mulhi_epi16(alpha_i_128,x_128[i]),2);
   #if defined(__x86_64__) || defined(__i386__)
      y_128[j]   = _mm_adds_epi16(y_128[j],_mm_unpacklo_epi16(yr,yi));
      j++;
      y_128[j]   = _mm_adds_epi16(y_128[j],_mm_unpackhi_epi16(yr,yi));
      j++;
   #elif defined(__arm__)
      int16x8x2_t yint;
      yint = vzipq_s16(yr,yi);
      y_128[j]   = adds_int16(y_128[j],yint.val[0]);
      j++;
      y_128[j]   = adds_int16(y_128[j],yint.val[1]);
   
      j++;
   #endif
   }

   _mm_empty();
   _m_empty();

}

void main(void) {

   short filt16a_l1[16] = {
   20480,16384,12288,8192,4096,0,0,0,0,0,0,0,0,0,0,0};

   short filt8_l0[8] = {
   16384,8192,0,0,0,0,0,0};

   short filt8_l1[8] = {
   24576,16384,0,0,0,0,0,0};

   short filt8_l2[8] = {
   0,0,13107,9830,6554,3277,0,0};

   short filt8_l3[8] = {
   22938,19661,0,0,13107,9830,6554,3277};

   short filt8_m0[8] = {
0,8192,16384,8192,0,0,0,0};

   short filt8_mr0[8] = {
0,0,0,8192,16384,8192,0,-8192};

   // int rxdataF = -378803862;
   int16_t pilot[6] = {23170, 23170, -23170,-23170, 23170,23170}; // 23170, -23170, 23170,23170, 23170,23170
   int16_t rxdataF[6] = {5784,5782, 5785,-5783, 5785,-5784};
   int16_t *pil = (int16_t *)&pilot;
   int16_t *rxF = (int16_t *)&rxdataF; // (int16_t *)&rxdataF;

   printf("rxdmrs -> (%d,%d)\n",rxF[0],rxF[1]);


   int16_t ch[32] = {0};

   ch[0] = (int16_t)(((int32_t)pil[0]*rxF[0] - (int32_t)pil[1]*rxF[1])>>15); // -
   ch[1] = (int16_t)(((int32_t)pil[0]*rxF[1] + (int32_t)pil[1]*rxF[0])>>15); // +

   printf("%d\n", (int32_t)pil[0]*rxF[0] - (int32_t)pil[1]*rxF[1]);
   
   printf("rxdmrs -> (%d,%d)\npil -> (%d,%d)\nch ->  (%d,%d)\n",rxF[0],rxF[1],pil[0],pil[1],ch[0],ch[1]);

   int16_t dl_ch[16] = {0};
   int16_t *fl = filt8_l0;  //filt8_l0;
   int16_t *fml = filt8_m0;
   int16_t *fmm = filt8_mr0;


   multadd_real_vector_complex_scalar(fl,
                                      ch,
                                      dl_ch,
                                      8); //8

   for (int i= 0; i<4; i++)
      printf("dl_ch addr %p %d+j*%d\n", dl_ch+2*i, *(dl_ch+2*i), *(dl_ch+2*i+1));

   // ch += 2;
   rxF += 2;
   pil += 2;

   ch[0] = (int16_t)(((int32_t)pil[0]*rxF[0] - (int32_t)pil[1]*rxF[1])>>15);
   ch[1] = (int16_t)(((int32_t)pil[0]*rxF[1] + (int32_t)pil[1]*rxF[0])>>15);

   printf("pilot 1 : rxF - > (%d,%d) ch -> (%d,%d), pil -> (%d,%d) \n",rxF[0],rxF[1],ch[0],ch[1],pil[0],pil[1]);

   multadd_real_vector_complex_scalar(fml,
                                      ch,
                                      dl_ch,
                                      8);
   pil += 2;
   rxF += 2;
   //printf("dl_ch addr %p\n",dl_ch
      
   ch[0] = (int16_t)(((int32_t)pil[0]*rxF[0] - (int32_t)pil[1]*rxF[1])>>15);
   ch[1] = (int16_t)(((int32_t)pil[0]*rxF[1] + (int32_t)pil[1]*rxF[0])>>15);

   printf("pilot 2 : rxF - > (%d,%d) ch -> (%d,%d), pil -> (%d,%d) \n",rxF[0],rxF[1],ch[0],ch[1],pil[0],pil[1]);

   multadd_real_vector_complex_scalar(fmm,
                                      ch,
                                      dl_ch,
                                      8);
                                         
   
   for (int i= 0; i<8; i++)
      printf("dl_ch addr %p  i:%d %d+j*%d\n", dl_ch+2*i, 8193+i, *(dl_ch+2*i), *(dl_ch+2*i+1));



   #if 0
      int output_shift = 14;

      __m128i *dl_ch128, *rxdataF128,*rxdataF_comp128;
      __m128i mmtmpD0, mmtmpD1, mmtmpD2, mmtmpD3;

      // [-4095+j*4092, 4094+j*-4096, -4096+j*-4098, 4092+j*4091]
      int rxdataF_ext[4] = {268234753, -268431362, -268505088, 268111868};
      // [0+j*8176, 0+j*-8, 0+j*-8184, 0+j*-8184]
      int dl_ch_estimates_ext[4] = {535822336, -524288, -536346624, -536346624}; // 8176, 
      int rxdataF_comp[4] = {0};

      // (+, +) 535822344  (+, +); 
      // (-, +) 535887864  (+, +); 
      // (-, -) -535756808 (-, -); 
      // (+, -) -535822328 (-, -);
      // (0, -) -535822336 (-, -);
      // (0, +) 535822336  (+, +);

      dl_ch128          = (__m128i *)&dl_ch_estimates_ext[0];
      rxdataF128        = (__m128i *)&rxdataF_ext[0];
      rxdataF_comp128   = (__m128i *)&rxdataF_comp[0];

   // multiply by conjugated channel
      mmtmpD0 = _mm_madd_epi16(dl_ch128[0],rxdataF128[0]);
               
      //  print_ints("re", &mmtmpD0);
      //  print_ints("re_32", (int32_t *)&mmtmpD0);
      // print_ints("ch",&dl_ch128[0]);
      print_shorts("dl_ch_16:", (int16_t *)&dl_ch128[0]);
      // print_ints("rxF",&rxdataF128[0]);
      // printf("rxF_64 = [%lld %lld]\n", ((int64_t *)&rxdataF128[0])[0], ((int64_t *)&rxdataF128[0])[1]);
      //  print_ints("rxF_32", (int32_t *)&rxdataF128[0]);
      print_shorts("rxF_16:     ", (int16_t *)&rxdataF128[0]);

      // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[0],_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
      //  print_ints("im",&mmtmpD1);
      mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[0]);
      // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
      //  print_ints("re(shift)",&mmtmpD0);
      mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
      //  print_ints("im(shift)",&mmtmpD1);
      mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
      mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
      //        print_ints("c0",&mmtmpD2);
      //  print_ints("c1",&mmtmpD3);
      rxdataF_comp128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);

      print_shorts("rxF_comp_16:", (int16_t *)&rxdataF_comp128[0]);
   #endif

}
 
