#include "dci_nr.h"

/*ref 36-212 v8.6.0 , pp 8-9 */
/* the highest degree is set by default */

unsigned int             poly24a = 0x864cfb00;   // 1000 0110 0100 1100 1111 1011
												 // D^24 + D^23 + D^18 + D^17 + D^14 + D^11 + D^10 + D^7 + D^6 + D^5 + D^4 + D^3 + D + 1
unsigned int             poly24b = 0x80006300;   // 1000 0000 0000 0000 0110 0011
											     // D^24 + D^23 + D^6 + D^5 + D + 1
unsigned int             poly24c = 0xb2b11700;   // 1011 0010 1011 0001 0001 0111
												 // D^24+D^23+D^21+D^20+D^17+D^15+D^13+D^12+D^8+D^4+D^2+D+1

unsigned int             poly16 = 0x10210000;    // 0001 0000 0010 0001            D^16 + D^12 + D^5 + 1
unsigned int             poly12 = 0x80F00000;    // 1000 0000 1111                 D^12 + D^11 + D^3 + D^2 + D + 1
unsigned int             poly8 = 0x9B000000;     // 1001 1011                      D^8  + D^7  + D^4 + D^3 + D + 1
uint32_t poly6 = 0x84000000; // 10000100000... -> D^6+D^5+1
uint32_t poly11 = 0xc4200000; //11000100001000... -> D^11+D^10+D^9+D^5+1

#define USE_INTEL_CRC 0

static unsigned int        crc24aTable[256];
static unsigned int        crc24bTable[256];
static unsigned int        crc24cTable[256];
static unsigned short      crc16Table[256];
static unsigned short      crc12Table[256];
static unsigned short      crc11Table[256];
static unsigned char       crc8Table[256];
static unsigned char       crc6Table[256];
void crcTableInit (void)
{
  unsigned char c = 0;

  do {
    crc24aTable[c] = crcbit (&c, 1, poly24a);
    crc24bTable[c] = crcbit (&c, 1, poly24b);
    crc24cTable[c] = crcbit (&c, 1, poly24c);
    crc16Table[c] = (unsigned short) (crcbit (&c, 1, poly16) >> 16);
    crc12Table[c] = (unsigned short) (crcbit (&c, 1, poly12) >> 16);
    crc11Table[c] = (unsigned short) (crcbit (&c, 1, poly11) >> 16);
    crc8Table[c] = (unsigned char) (crcbit (&c, 1, poly8) >> 24);
    crc6Table[c] = (unsigned char) (crcbit (&c, 1, poly6) >> 24);
  } while (++c);
#if USE_INTEL_CRC
    crc_xmm_be_le_swap128 = _mm_setr_epi32(0x0c0d0e0f, 0x08090a0b,
					   0x04050607, 0x00010203);

#endif
}


/*********************************************************

Byte by byte LUT implementations,
assuming initial byte is 0 padded (in MSB) if necessary
can use SIMD optimized Intel CRC for LTE/NR 24a/24b variants
*********************************************************/

unsigned int crc24a (unsigned char * inptr,
					 int bitlen)
{
  int octetlen = bitlen / 8;  /* Change in octets */

  if ( bitlen % 8 || !USE_INTEL_CRC ) {
  unsigned int      crc = 0;
  int resbit= (bitlen % 8);
    
  while (octetlen-- > 0) {
    //   printf("crc24a: in %x => crc %x\n",crc,*inptr);
    crc = (crc << 8) ^ crc24aTable[(*inptr++) ^ (crc >> 24)];
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ crc24aTable[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))];
  return crc;
  }
  #if USE_INTEL_CRC
  else {
  return crc32_calc_pclmulqdq(inptr, octetlen, 0,
                              &lte_crc24a_pclmulqdq);
  }
  #endif

}

#if USE_INTEL_CRC
static DECLARE_ALIGNED(struct crc_pclmulqdq_ctx lte_crc24b_pclmulqdq, 16) = {
        0x80140500,     /**< k1 */
        0x42000100,     /**< k2 */
        0x90042100,     /**< k3 */
        0xffff83ff,     /**< q */
        0x80006300,     /**< p */
        0ULL            /**< res */
};
#endif
unsigned int crc24b (unsigned char * inptr,
	   	     int bitlen)
{
  int octetlen = bitlen / 8;  /* Change in octets */
  
  if ( bitlen % 8 || !USE_INTEL_CRC ) {
  unsigned int crc = 0;
  int resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    //    printf("crc24b: in %x => crc %x (%x)\n",crc,*inptr,crc24bTable[(*inptr) ^ (crc >> 24)]);
    crc = (crc << 8) ^ crc24bTable[(*inptr++) ^ (crc >> 24)];
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ crc24bTable[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))];

  return crc;
  }
#if USE_INTEL_CRC
  else {
  return crc32_calc_pclmulqdq(inptr, octetlen, 0,
                              &lte_crc24b_pclmulqdq);
  }
#endif
}

unsigned int crc24c (unsigned char * inptr,
					 int bitlen)
{
  int octetlen, resbit;
  unsigned int crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    crc = (crc << 8) ^ crc24cTable[(*inptr++) ^ (crc >> 24)];
  }

  if (resbit > 0) {
    crc = (crc << resbit) ^ crc24cTable[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))];
  }

  return crc;
}

unsigned int
crc16 (unsigned char * inptr, int bitlen)
{
  int             octetlen, resbit;
  unsigned int             crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {

    crc = (crc << 8) ^ (crc16Table[(*inptr++) ^ (crc >> 24)] << 16);
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ (crc16Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 16);

  return crc;
}

unsigned int
crc12 (unsigned char * inptr, int bitlen)
{
  int             octetlen, resbit;
  unsigned int             crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    crc = (crc << 8) ^ (crc12Table[(*inptr++) ^ (crc >> 24)] << 16);
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ (crc12Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 16);

  return crc;
}

unsigned int
crc11 (unsigned char * inptr, int bitlen)
{
  int             octetlen, resbit;
  unsigned int             crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    crc = (crc << 8) ^ (crc11Table[(*inptr++) ^ (crc >> 24)] << 16);
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ (crc11Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 16);

  return crc;
}

unsigned int
crc8 (unsigned char * inptr, int bitlen)
{
  int             octetlen, resbit;
  unsigned int             crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    crc = crc8Table[(*inptr++) ^ (crc >> 24)] << 24;
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ (crc8Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 24);

  return crc;
}

unsigned int
crc6 (unsigned char * inptr, int bitlen)
{
  int             octetlen, resbit;
  unsigned int             crc = 0;
  octetlen = bitlen / 8;        /* Change in octets */
  resbit = (bitlen % 8);

  while (octetlen-- > 0) {
    crc = crc6Table[(*inptr++) ^ (crc >> 24)] << 24;
  }

  if (resbit > 0)
    crc = (crc << resbit) ^ (crc6Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 24);

  return crc;
}

int check_crc(uint8_t* decoded_bytes, uint32_t n, uint32_t F, uint8_t crc_type)
{
  uint32_t crc=0,oldcrc=0;
  uint8_t crc_len=0;

  switch (crc_type) {
  case CRC24_A:
  case CRC24_B:
    crc_len=3;
    break;

  case CRC16:
    crc_len=2;
    break;

  case CRC8:
    crc_len=1;
    break;

  default:
    AssertFatal(1,"Invalid crc_type \n");
  }

  for (int i=0; i<crc_len; i++)
    oldcrc |= (decoded_bytes[(n>>3)-crc_len+i])<<((crc_len-1-i)<<3);

  switch (crc_type) {
    
  case CRC24_A:
    oldcrc&=0x00ffffff;
    crc = crc24a(decoded_bytes,
		 n-24)>>8;
    
    break;
    
  case CRC24_B:
      oldcrc&=0x00ffffff;
      crc = crc24b(decoded_bytes,
                   n-24)>>8;
      
      break;

    case CRC16:
      oldcrc&=0x0000ffff;
      crc = crc16(decoded_bytes,
                  n-16)>>16;
      
      break;

    case CRC8:
      oldcrc&=0x000000ff;
      crc = crc8(decoded_bytes,
                 n-8)>>24;
      break;

    default:
      AssertFatal(1,"Invalid crc_type \n");
    }

    printf("crc = 0x%x oldcrc = 0x%x\n", crc, oldcrc);
    if (crc == oldcrc)
      return(1);
    else
      return(0);

}

static int intcmp(const void *p1,const void *p2) {
  return(*(int16_t *)p1 > *(int16_t *)p2);
}

uint32_t polar_decoder_int16(int16_t *input,
                             uint64_t *out,
                             uint8_t ones_flag,
                             const t_nrPolar_params *polarParams)
{
  
  int16_t d_tilde[polarParams->N];// = malloc(sizeof(double) * polarParams->N);
  nr_polar_rate_matching_int16(input, d_tilde, polarParams->rate_matching_pattern, polarParams->K, polarParams->N, polarParams->encoderLength);

  for (int i=0; i<polarParams->N; i++) {
    if (d_tilde[i]<-128) d_tilde[i]=-128;
    else if (d_tilde[i]>127) d_tilde[i]=128;
  }

  memcpy((void *)&polarParams->tree.root->alpha[0],(void *)&d_tilde[0],sizeof(int16_t)*polarParams->N);
  printf("polarParams->N %d polarParams->tree.root->alpha[0] %p d_tilde[0] = %d\n", polarParams->N, polarParams->tree.root->alpha[0], d_tilde[0]);
  generic_polar_decoder(polarParams,polarParams->tree.root);

  //Extract the information bits (û to ĉ)
  uint64_t Cprime[4]= {0,0,0,0};
  uint64_t B[4]= {0,0,0,0};

  for (int i=0; i<polarParams->K; i++) Cprime[i>>6] = Cprime[i>>6] | ((uint64_t)polarParams->nr_polar_U[polarParams->Q_I_N[i]])<<(i&63);

  //Deinterleaving (ĉ to b)
  uint8_t *Cprimebyte = (uint8_t *)Cprime;

  if (polarParams->K<65) {
    B[0] = polarParams->B_tab0[0][Cprimebyte[0]] |
           polarParams->B_tab0[1][Cprimebyte[1]] |
           polarParams->B_tab0[2][Cprimebyte[2]] |
           polarParams->B_tab0[3][Cprimebyte[3]] |
           polarParams->B_tab0[4][Cprimebyte[4]] |
           polarParams->B_tab0[5][Cprimebyte[5]] |
           polarParams->B_tab0[6][Cprimebyte[6]] |
           polarParams->B_tab0[7][Cprimebyte[7]];
  } else if (polarParams->K<129) {
    int len = polarParams->K/8;

    if ((polarParams->K&7) > 0) len++;

    for (int k=0; k<len; k++) {
      B[0] |= polarParams->B_tab0[k][Cprimebyte[k]];
      B[1] |= polarParams->B_tab1[k][Cprimebyte[k]];
    }
  }

  int len=polarParams->payloadBits;
  //int len_mod64=len&63;
  int crclen = polarParams->crcParityBits;
  uint64_t rxcrc=B[0]&((1<<crclen)-1);
  uint32_t crc = 0;
  uint64_t Ar = 0;
  AssertFatal(len<65,"A must be less than 65 bits\n");

  // appending 24 ones before a0 for DCI as stated in 38.212 7.3.2
  uint8_t offset = 0;
  if (ones_flag) offset = 3;

  if (len<=32) {
    Ar = (uint32_t)(B[0]>>crclen);
    uint8_t A32_flip[4+offset];
    if (ones_flag) {
      A32_flip[0] = 0xff;
      A32_flip[1] = 0xff;
      A32_flip[2] = 0xff;
    }
    uint32_t Aprime= (uint32_t)(Ar<<(32-len));
    A32_flip[0+offset]=((uint8_t *)&Aprime)[3];
    A32_flip[1+offset]=((uint8_t *)&Aprime)[2];
    A32_flip[2+offset]=((uint8_t *)&Aprime)[1];
    A32_flip[3+offset]=((uint8_t *)&Aprime)[0];
    if      (crclen == 24) crc = (uint64_t)((crc24c(A32_flip,8*offset+len)>>8)&0xffffff);
    else if (crclen == 11) crc = (uint64_t)((crc11(A32_flip,8*offset+len)>>21)&0x7ff);
    else if (crclen == 6)  crc = (uint64_t)((crc6(A32_flip,8*offset+len)>>26)&0x3f);
  } else if (len<=64) {
    Ar = (B[0]>>crclen) | (B[1]<<(64-crclen));;
    uint8_t A64_flip[8+offset];
    if (ones_flag) {
      A64_flip[0] = 0xff;
      A64_flip[1] = 0xff;
      A64_flip[2] = 0xff;
    }
    uint64_t Aprime= (uint64_t)(Ar<<(64-len));
    A64_flip[0+offset]=((uint8_t *)&Aprime)[7];
    A64_flip[1+offset]=((uint8_t *)&Aprime)[6];
    A64_flip[2+offset]=((uint8_t *)&Aprime)[5];
    A64_flip[3+offset]=((uint8_t *)&Aprime)[4];
    A64_flip[4+offset]=((uint8_t *)&Aprime)[3];
    A64_flip[5+offset]=((uint8_t *)&Aprime)[2];
    A64_flip[6+offset]=((uint8_t *)&Aprime)[1];
    A64_flip[7+offset]=((uint8_t *)&Aprime)[0];
    if      (crclen==24) crc = (uint64_t)(crc24c(A64_flip,8*offset+len)>>8)&0xffffff;
    else if (crclen==11) crc = (uint64_t)(crc11(A64_flip,8*offset+len)>>21)&0x7ff;
    else if (crclen==6) crc = (uint64_t)(crc6(A64_flip,8*offset+len)>>26)&0x3f;
  }

#if 0
  printf("A %llx B %llx|%llx Cprime %llx|%llx  (crc %x,rxcrc %llx %d)\n",
         Ar,
         B[1],B[0],Cprime[1],Cprime[0],crc,
         rxcrc,polarParams->payloadBits);
#endif
  out[0]=Ar;
  return(crc^rxcrc);
}

void nr_pdcch_unscrambling(int16_t *z,
                           uint16_t scrambling_RNTI,
                           uint32_t length,
                           uint16_t pdcch_DMRS_scrambling_id,
                           int16_t *z2) {


  int i;
  uint8_t reset;
  uint32_t x1, x2, s = 0;
  uint16_t n_id; //{0,1,...,65535}
  uint32_t rnti = (uint32_t) scrambling_RNTI;
  reset = 1;
  // x1 is set in first call to lte_gold_generic
  n_id = pdcch_DMRS_scrambling_id;
  x2 = ((rnti<<16) + n_id); //mod 2^31 is implicit //this is c_init in 38.211 v15.1.0 Section 7.3.2.3

  // printf("PDCCH Unscrambling x2 %x : scrambling_RNTI %x\n", x2, rnti);

  for (i = 0; i < length; i++) {
    if ((i & 0x1f) == 0) {
      s = lte_gold_generic(&x1, &x2, reset);
      reset = 0;
    }

    if (((s >> (i % 32)) & 1) == 1) z2[i] = -z[i];
    else z2[i]=z[i];
  }
}

void nr_polar_rate_matching_int16(int16_t *input,
				  int16_t *output,
				  uint16_t *rmp,
				  uint16_t K,
				  uint16_t N,
				  uint16_t E)
{
  if (E>=N) { //repetition
    memset((void*)output,0,N*sizeof(int16_t));
    for (int i=0; i<=E-1; i++) output[rmp[i]]+=input[i];
  } else {
    if ( (K/(double)E) <= (7.0/16) ) memset((void*)output,0,N*sizeof(int16_t)); //puncturing
    else { //shortening
      for (int i=0; i<=N-1; i++) output[i]=32767;//instead of INFINITY, to prevent [-Woverflow]
    }

    for (int i=0; i<=E-1; i++) output[rmp[i]]=input[i];
   
  }

}

const uint16_t* nr_polar_sequence_pattern(uint8_t n){
	return Q_0_Nminus1[n];
}
uint16_t nr_polar_aggregation_prime (uint8_t aggregation_level) {
  if (aggregation_level == 0) return 0;
  else if (aggregation_level == 1) return NR_POLAR_AGGREGATION_LEVEL_1_PRIME;
  else if (aggregation_level == 2) return NR_POLAR_AGGREGATION_LEVEL_2_PRIME;
  else if (aggregation_level == 4) return NR_POLAR_AGGREGATION_LEVEL_4_PRIME;
  else if (aggregation_level == 8) return NR_POLAR_AGGREGATION_LEVEL_8_PRIME;
  else return NR_POLAR_AGGREGATION_LEVEL_16_PRIME; //aggregation_level == 16
}

// ----- Old implementation ----
uint8_t **crc24c_generator_matrix(uint16_t payloadSizeBits){

	uint8_t crcPolynomialPattern[25] = {1,1,0,1,1,0,0,1,0,1,0,1,1,0,0,0,1,0,0,0,1,0,1,1,1};
	// 1011 0010 1011 0001 0001 0111 D^24 + D^23 + D^21 + D^20 + D^17 + D^15 + D^13 + D^12 + D^8 + D^4 + D^2 + D + 1
	uint8_t crcPolynomialSize = 24;
	uint8_t temp1[crcPolynomialSize], temp2[crcPolynomialSize];

	uint8_t **crc_generator_matrix = malloc(payloadSizeBits * sizeof(uint8_t *));
	if (crc_generator_matrix)
	  for (int i = 0; i < payloadSizeBits; i++)
		  crc_generator_matrix[i] = malloc(crcPolynomialSize * sizeof(uint8_t));

	for (int i = 0; i < crcPolynomialSize; i++) crc_generator_matrix[payloadSizeBits-1][i]=crcPolynomialPattern[i+1];

	for (int i = payloadSizeBits-2; i >= 0; i--){
		for (int j = 0; j < crcPolynomialSize-1; j++) temp1[j]=crc_generator_matrix[i+1][j+1];

		temp1[crcPolynomialSize-1]=0;

		for (int j = 0; j < crcPolynomialSize; j++)
			temp2[j]=crc_generator_matrix[i+1][0]*crcPolynomialPattern[j+1];

		for (int j = 0; j < crcPolynomialSize; j++){
			if(temp1[j]+temp2[j] == 1)
				crc_generator_matrix[i][j]=1;
			else
				crc_generator_matrix[i][j]=0;
		}
	}
	return crc_generator_matrix;
}
uint32_t nr_polar_output_length(uint16_t K,
				uint16_t E,
				uint8_t n_max)
{
  uint8_t n_1, n_2, n_min=5, n;
  double R_min=1.0/8;
  
  if ( (E <= (9.0/8)*pow(2,ceil(log2(E))-1)) && (K/E < 9.0/16) ) {
    n_1 = ceil(log2(E))-1;
  } else {
    n_1 = ceil(log2(E));
  }
  
  n_2 = ceil(log2(K/R_min));
  
  n=n_max;
  if (n>n_1) n=n_1;
  if (n>n_2) n=n_2;
  if (n<n_min) n=n_min;

  /*printf("nr_polar_output_length: K %d, E %d, n %d (n_max %d,n_min %d, n_1 %d,n_2 %d)\n",
	 K,E,n,n_max,n_min,n_1,n_2);
	 exit(-1);*/
  return ((uint32_t) pow(2.0,n)); //=polar_code_output_length
}
void nr_polar_interleaving_pattern(uint16_t K, uint8_t I_IL, uint16_t *PI_k_){
	uint8_t K_IL_max=164, k=0;
	uint8_t interleaving_pattern_table[164] = {0, 2, 4, 7, 9, 14, 19, 20, 24, 25, 26, 28, 31, 34,
			42, 45, 49, 50, 51, 53, 54, 56, 58, 59, 61, 62, 65, 66,
			67, 69, 70, 71, 72, 76, 77, 81, 82, 83, 87, 88, 89, 91,
			93, 95, 98, 101, 104, 106, 108, 110, 111, 113, 115, 118, 119, 120,
			122, 123, 126, 127, 129, 132, 134, 138, 139, 140, 1, 3, 5, 8,
			10, 15, 21, 27, 29, 32, 35, 43, 46, 52, 55, 57, 60, 63,
			68, 73, 78, 84, 90, 92, 94, 96, 99, 102, 105, 107, 109, 112,
			114, 116, 121, 124, 128, 130, 133, 135, 141, 6, 11, 16, 22, 30,
			33, 36, 44, 47, 64, 74, 79, 85, 97, 100, 103, 117, 125, 131,
			136, 142, 12, 17, 23, 37, 48, 75, 80, 86, 137, 143, 13, 18,
			38, 144, 39, 145, 40, 146, 41, 147, 148, 149, 150, 151, 152, 153,
			154, 155, 156, 157, 158, 159, 160, 161, 162, 163};
	
	if (I_IL == 0){
		for (; k<= K-1; k++)
			PI_k_[k] = k;
	} else {
		for (int m=0; m<= (K_IL_max-1); m++){
			if (interleaving_pattern_table[m] >= (K_IL_max-K)) {
				PI_k_[k] = interleaving_pattern_table[m]-(K_IL_max-K);
				k++;
			}
		}
	}
}

void nr_polar_rate_matching_pattern(uint16_t *rmp,
				    uint16_t *J,
				    const uint8_t *P_i_,
				    uint16_t K,
				    uint16_t N,
				    uint16_t E)
{
  uint8_t i;
  uint16_t *d, *y, ind;
  d = (uint16_t *)malloc(sizeof(uint16_t) * N);
  y = (uint16_t *)malloc(sizeof(uint16_t) * N);

  for (int m=0; m<=N-1; m++) d[m]=m;

  for (int m=0; m<=N-1; m++){
    i=floor((32*m)/N);
    J[m] = (P_i_[i]*(N/32)) + (m%(N/32));
    y[m] = d[J[m]];
  }

  if (E>=N) { //repetition
    for (int k=0; k<=E-1; k++) {
      ind = (k%N);
      rmp[k]=y[ind];
    }
  } else {
    if ( (K/(double)E) <= (7.0/16) ) { //puncturing
      for (int k=0; k<=E-1; k++) {
	rmp[k]=y[k+N-E];
      }
    } else { //shortening
      for (int k=0; k<=E-1; k++) {
	rmp[k]=y[k];
      }
    }
  }

  free(d);
  free(y);
}

void nr_polar_info_bit_pattern(uint8_t *ibp,
			       int16_t *Q_I_N,
			       int16_t *Q_F_N,
			       uint16_t *J,
			       const uint16_t *Q_0_Nminus1,
			       uint16_t K,
			       uint16_t N,
			       uint16_t E,
			       uint8_t n_PC)
{
  int16_t *Q_Ftmp_N = malloc(sizeof(int16_t) * (N + 1)); // Last element shows the final
  int16_t *Q_Itmp_N = malloc(sizeof(int16_t) * (N + 1)); // array index assigned a value.

  for (int i = 0; i <= N; i++) {
    Q_Ftmp_N[i] = -1; // Empty array.
    Q_Itmp_N[i] = -1;
  }

  uint8_t flag;
  uint16_t limit, ind;

  if (E < N) {
    if ((K / (double) E) <= (7.0 / 16)) { //puncturing
      for (int n = 0; n <= N - E - 1; n++) {
	ind = Q_Ftmp_N[N] + 1;
	Q_Ftmp_N[ind] = J[n];
	Q_Ftmp_N[N] = Q_Ftmp_N[N] + 1;
      }

      if ((E / (double) N) >= (3.0 / 4)) {
	limit = ceil((double) (3 * N - 2 * E) / 4);
	for (int n = 0; n <= limit - 1; n++) {
	  ind = Q_Ftmp_N[N] + 1;
	  Q_Ftmp_N[ind] = n;
	  Q_Ftmp_N[N] = Q_Ftmp_N[N] + 1;
	}
      } else {
	limit = ceil((double) (9 * N - 4 * E) / 16);
	for (int n = 0; n <= limit - 1; n++) {
	  ind = Q_Ftmp_N[N] + 1;
	  Q_Ftmp_N[ind] = n;
	  Q_Ftmp_N[N] = Q_Ftmp_N[N] + 1;
	}
      }
    } else { //shortening
      for (int n = E; n <= N - 1; n++) {
	ind = Q_Ftmp_N[N] + 1;
	Q_Ftmp_N[ind] = J[n];
	Q_Ftmp_N[N] = Q_Ftmp_N[N] + 1;
      }
    }
  }

  //Q_I,tmp_N = Q_0_N-1 \ Q_F,tmp_N
  for (int n = 0; n <= N - 1; n++) {
    flag = 1;
    for (int m = 0; m <= Q_Ftmp_N[N]; m++) {
      if (Q_0_Nminus1[n] == Q_Ftmp_N[m]) {
	flag = 0;
	break;
      }
    }
    if (flag) {
      Q_Itmp_N[Q_Itmp_N[N] + 1] = Q_0_Nminus1[n];
      Q_Itmp_N[N]++;
    }
  }

  //Q_I_N comprises (K+n_PC) most reliable bit indices in Q_I,tmp_N
  for (int n = 0; n <= (K + n_PC) - 1; n++) {
    ind = Q_Itmp_N[N] + n - ((K + n_PC) - 1);
    Q_I_N[n] = Q_Itmp_N[ind];
  }

  //Q_F_N = Q_0_N-1 \ Q_I_N
  for (int n = 0; n <= N - 1; n++) {
    flag = 1;
    for (int m = 0; m <= (K + n_PC) - 1; m++) {
      if (Q_0_Nminus1[n] == Q_I_N[m]) {
	flag = 0;
	break;
      }
    }
    if (flag) {
      Q_F_N[Q_F_N[N] + 1] = Q_0_Nminus1[n];
      Q_F_N[N]++;
    }
  }

  //Information Bit Pattern
  for (int n = 0; n <= N - 1; n++) {
    ibp[n] = 0;

    for (int m = 0; m <= (K + n_PC) - 1; m++) {
      if (n == Q_I_N[m]) {
	ibp[n] = 1;
	break;
      }
    }
  }

  free(Q_Ftmp_N);
  free(Q_Itmp_N);
}

t_nrPolar_params *nr_polar_params (int8_t messageType,
                                   uint16_t messageLength,
                                   uint8_t aggregation_level,
	 		           int decoding_flag,
				   t_nrPolar_params **polarList_ext) {
  static t_nrPolar_params *polarList = NULL;
  nr_polar_init(polarList_ext != NULL ? polarList_ext : &polarList, 
		messageType,messageLength,aggregation_level,decoding_flag);
  t_nrPolar_params *polarParams=polarList_ext != NULL ? *polarList_ext : polarList;
  const int tag=messageType * messageLength * (messageType>=2 ? aggregation_level : nr_polar_aggregation_prime(aggregation_level));


	
  while (polarParams != NULL) {
       printf("nr_polar_params : tag %d (from nr_polar_init %d)\n",tag,polarParams->idx);
    if (polarParams->idx == tag)
      return polarParams;

    polarParams = polarParams->nextPtr;
  }

  AssertFatal(0,"Polar Init tables internal failure, no polarParams found\n");
  return NULL;
}


static void nr_polar_init(t_nrPolar_params * *polarParams,
                          int8_t messageType,
                          uint16_t messageLength,
                          uint8_t aggregation_level,
			  int decoder_flag) {
  t_nrPolar_params *currentPtr = *polarParams;
  uint16_t aggregation_prime = (messageType >= 2) ? aggregation_level : nr_polar_aggregation_prime(aggregation_level);
  //Parse the list. If the node is already created, return without initialization.
  while (currentPtr != NULL) {
    // printf("currentPtr->idx %d, (%d,%d)\n",currentPtr->idx,currentPtr->payloadBits,currentPtr->encoderLength);
    //LOG_D(PHY,"Looking for index %d\n",(messageType * messageLength * aggregation_prime));
    if (currentPtr->idx == (messageType * messageLength * aggregation_prime)) return;
    else currentPtr = currentPtr->nextPtr;
  }

  //  printf("currentPtr %p (polarParams %p)\n",currentPtr,polarParams);
  //Else, initialize and add node to the end of the linked list.
  t_nrPolar_params *newPolarInitNode = calloc(sizeof(t_nrPolar_params),1);
 
  if (newPolarInitNode != NULL) {
    //   LOG_D(PHY,"Setting new polarParams index %d, messageType %d, messageLength %d, aggregation_prime %d\n",(messageType * messageLength * aggregation_prime),messageType,messageLength,aggregation_prime);
    newPolarInitNode->idx = (messageType * messageLength * aggregation_prime);
    newPolarInitNode->nextPtr = NULL;
    //printf("newPolarInitNode->idx %d, (%d,%d,%d:%d)\n",newPolarInitNode->idx,messageType,messageLength,aggregation_prime,aggregation_level);

    if (messageType == 0) { //PBCH
      newPolarInitNode->n_max = NR_POLAR_PBCH_N_MAX;
      newPolarInitNode->i_il = NR_POLAR_PBCH_I_IL;
      newPolarInitNode->i_seg = NR_POLAR_PBCH_I_SEG;
      newPolarInitNode->n_pc = NR_POLAR_PBCH_N_PC;
      newPolarInitNode->n_pc_wm = NR_POLAR_PBCH_N_PC_WM;
      newPolarInitNode->i_bil = NR_POLAR_PBCH_I_BIL;
      newPolarInitNode->crcParityBits = NR_POLAR_PBCH_CRC_PARITY_BITS;
      newPolarInitNode->payloadBits = NR_POLAR_PBCH_PAYLOAD_BITS;
      newPolarInitNode->encoderLength = NR_POLAR_PBCH_E;
      newPolarInitNode->crcCorrectionBits = NR_POLAR_PBCH_CRC_ERROR_CORRECTION_BITS;
      newPolarInitNode->crc_generator_matrix = crc24c_generator_matrix(newPolarInitNode->payloadBits);//G_P
      //printf("Initializing polar parameters for PBCH (K %d, E %d)\n",newPolarInitNode->payloadBits,newPolarInitNode->encoderLength);
    } else if (messageType == 1) { //DCI
      newPolarInitNode->n_max = NR_POLAR_DCI_N_MAX;
      newPolarInitNode->i_il = NR_POLAR_DCI_I_IL;
      newPolarInitNode->i_seg = NR_POLAR_DCI_I_SEG;
      newPolarInitNode->n_pc = NR_POLAR_DCI_N_PC;
      newPolarInitNode->n_pc_wm = NR_POLAR_DCI_N_PC_WM;
      newPolarInitNode->i_bil = NR_POLAR_DCI_I_BIL;
      newPolarInitNode->crcParityBits = NR_POLAR_DCI_CRC_PARITY_BITS;
      newPolarInitNode->payloadBits = messageLength;
      newPolarInitNode->encoderLength = aggregation_level*108;
      newPolarInitNode->crcCorrectionBits = NR_POLAR_DCI_CRC_ERROR_CORRECTION_BITS;
      newPolarInitNode->crc_generator_matrix=crc24c_generator_matrix(newPolarInitNode->payloadBits+newPolarInitNode->crcParityBits);//G_P
      //printf("Initializing polar parameters for DCI (K %d, E %d, L %d)\n",newPolarInitNode->payloadBits,newPolarInitNode->encoderLength,aggregation_level);
    } else if (messageType == 2) { //UCI PUCCH2
      AssertFatal(aggregation_level>2,"Aggregation level (%d) for PUCCH 2 encoding is NPRB and should be > 2\n",aggregation_level);
      AssertFatal(messageLength>11,"Message length %d is too short for polar encoding of UCI\n",messageLength);
      newPolarInitNode->n_max = NR_POLAR_PUCCH_N_MAX;
      newPolarInitNode->i_il = NR_POLAR_PUCCH_I_IL;
      newPolarInitNode->encoderLength = aggregation_level * 16;

      newPolarInitNode->i_seg = 0;
      
      if ((messageLength >= 360 && newPolarInitNode->encoderLength >= 1088)||
	  (messageLength >= 1013)) newPolarInitNode->i_seg = 1;

      newPolarInitNode->crcParityBits = 11;
      newPolarInitNode->n_pc = 0;
      newPolarInitNode->n_pc_wm = 0;

      if (messageLength < 20) {
	newPolarInitNode->crcParityBits = 6;
	newPolarInitNode->n_pc = 3;
	if ((newPolarInitNode->encoderLength - messageLength - 6 + 3) < 193) newPolarInitNode->n_pc_wm = 1; 
      }



      newPolarInitNode->i_bil = NR_POLAR_PUCCH_I_BIL;

      newPolarInitNode->payloadBits = messageLength;
      newPolarInitNode->crcCorrectionBits = NR_POLAR_PUCCH_CRC_ERROR_CORRECTION_BITS;
      //newPolarInitNode->crc_generator_matrix=crc24c_generator_matrix(newPolarInitNode->payloadBits+newPolarInitNode->crcParityBits);//G_P
      //LOG_D(PHY,"New polar node, encoderLength %d, aggregation_level %d\n",newPolarInitNode->encoderLength,aggregation_level);
    } else {
      AssertFatal(1 == 0, "[nr_polar_init] Incorrect Message Type(%d)", messageType);
    }

    newPolarInitNode->K = newPolarInitNode->payloadBits + newPolarInitNode->crcParityBits; // Number of bits to encode.
    newPolarInitNode->N = nr_polar_output_length(newPolarInitNode->K,
						 newPolarInitNode->encoderLength,
						 newPolarInitNode->n_max);
    newPolarInitNode->n = log2(newPolarInitNode->N);
    newPolarInitNode->G_N = nr_polar_kronecker_power_matrices(newPolarInitNode->n);
    //polar_encoder vectors:
    newPolarInitNode->nr_polar_crc = malloc(sizeof(uint8_t) * newPolarInitNode->crcParityBits);
    newPolarInitNode->nr_polar_aPrime = malloc(sizeof(uint8_t) * ((ceil((newPolarInitNode->payloadBits)/32.0)*4)+3));
    newPolarInitNode->nr_polar_APrime = malloc(sizeof(uint8_t) * newPolarInitNode->K);
    newPolarInitNode->nr_polar_D = malloc(sizeof(uint8_t) * newPolarInitNode->N);
    newPolarInitNode->nr_polar_E = malloc(sizeof(uint8_t) * newPolarInitNode->encoderLength);
    //Polar Coding vectors
    newPolarInitNode->nr_polar_U = malloc(sizeof(uint8_t) * newPolarInitNode->N); //Decoder: nr_polar_uHat
    newPolarInitNode->nr_polar_CPrime = malloc(sizeof(uint8_t) * newPolarInitNode->K); //Decoder: nr_polar_cHat
    newPolarInitNode->nr_polar_B = malloc(sizeof(uint8_t) * newPolarInitNode->K); //Decoder: nr_polar_bHat
    newPolarInitNode->nr_polar_A = malloc(sizeof(uint8_t) * newPolarInitNode->payloadBits); //Decoder: nr_polar_aHat
    newPolarInitNode->Q_0_Nminus1 = nr_polar_sequence_pattern(newPolarInitNode->n);
    newPolarInitNode->interleaving_pattern = malloc(sizeof(uint16_t) * newPolarInitNode->K);
    nr_polar_interleaving_pattern(newPolarInitNode->K,
                                  newPolarInitNode->i_il,
                                  newPolarInitNode->interleaving_pattern);
    newPolarInitNode->deinterleaving_pattern = malloc(sizeof(uint16_t) * newPolarInitNode->K);

    for (int i=0; i<newPolarInitNode->K; i++)
      newPolarInitNode->deinterleaving_pattern[newPolarInitNode->interleaving_pattern[i]] = i;

    newPolarInitNode->rate_matching_pattern = malloc(sizeof(uint16_t) * newPolarInitNode->encoderLength);
    uint16_t *J = malloc(sizeof(uint16_t) * newPolarInitNode->N);
    nr_polar_rate_matching_pattern(newPolarInitNode->rate_matching_pattern,
                                   J,
                                   nr_polar_subblock_interleaver_pattern,
                                   newPolarInitNode->K,
                                   newPolarInitNode->N,
                                   newPolarInitNode->encoderLength);
    newPolarInitNode->information_bit_pattern = malloc(sizeof(uint8_t) * newPolarInitNode->N);
    newPolarInitNode->Q_I_N = malloc(sizeof(int16_t) * (newPolarInitNode->K + newPolarInitNode->n_pc));
    newPolarInitNode->Q_F_N = malloc( sizeof(int16_t) * (newPolarInitNode->N + 1)); // Last element shows the final array index assigned a value.
    newPolarInitNode->Q_PC_N = malloc( sizeof(int16_t) * (newPolarInitNode->n_pc));

    for (int i = 0; i <= newPolarInitNode->N; i++)
      newPolarInitNode->Q_F_N[i] = -1; // Empty array.

    nr_polar_info_bit_pattern(newPolarInitNode->information_bit_pattern,
                              newPolarInitNode->Q_I_N,
                              newPolarInitNode->Q_F_N,
                              J,
                              newPolarInitNode->Q_0_Nminus1,
                              newPolarInitNode->K,
                              newPolarInitNode->N,
                              newPolarInitNode->encoderLength,
                              newPolarInitNode->n_pc);
    // sort the Q_I_N array in ascending order (first K positions)
    qsort((void *)newPolarInitNode->Q_I_N,newPolarInitNode->K,sizeof(int16_t),intcmp);
    newPolarInitNode->channel_interleaver_pattern = malloc(sizeof(uint16_t) * newPolarInitNode->encoderLength);
    nr_polar_channel_interleaver_pattern(newPolarInitNode->channel_interleaver_pattern,
                                         newPolarInitNode->i_bil,
                                         newPolarInitNode->encoderLength);
    free(J);
    if (decoder_flag == 1) build_decoder_tree(newPolarInitNode);
    build_polar_tables(newPolarInitNode);
    init_polar_deinterleaver_table(newPolarInitNode);
    //printf("decoder tree nodes %d\n",newPolarInitNode->tree.num_nodes);
  } else {
    AssertFatal(1 == 0, "[nr_polar_init] New t_nrPolar_params * could not be created");
  }

  //Fixme: the list is not thread safe
  //The defect is not critical: we always append (never delete items) and adding two times the same is fine
  newPolarInitNode->nextPtr=*polarParams;
  *polarParams=newPolarInitNode;
  return;
}

void init_polar_deinterleaver_table(t_nrPolar_params *polarParams) {
  AssertFatal(polarParams->K > 17, "K = %d < 18, is not allowed\n",polarParams->K);
  AssertFatal(polarParams->K < 129, "K = %d > 128, is not supported yet\n",polarParams->K);
  int bit_i,ip,ipmod64;
  int numbytes = polarParams->K>>3;
  int residue = polarParams->K&7;
  int numbits;

  if (residue>0) numbytes++;

  for (int byte=0; byte<numbytes; byte++) {
    if (byte<(polarParams->K>>3)) numbits=8;
    else numbits=residue;

    for (int i=0; i<numbits; i++) {
      // flip bit endian for B
      ip=polarParams->K - 1 - polarParams->interleaving_pattern[(8*byte)+i];
#if 0
      printf("byte %d, i %d => ip %d\n",byte,i,ip);
#endif
      ipmod64 = ip&63;
      AssertFatal(ip<128,"ip = %d\n",ip);

      for (int val=0; val<256; val++) {
        bit_i=(val>>i)&1;

        if (ip<64) polarParams->B_tab0[byte][val] |= (((uint64_t)bit_i)<<ipmod64);
        else       polarParams->B_tab1[byte][val] |= (((uint64_t)bit_i)<<ipmod64);
      }
    }
  }
}

void build_polar_tables(t_nrPolar_params *polarParams) {
  // build table b -> c'
  AssertFatal(polarParams->K > 17, "K = %d < 18, is not possible\n",polarParams->K);
  AssertFatal(polarParams->K < 129, "K = %d > 128, is not supported yet\n",polarParams->K);
  int bit_i,ip;
  int numbytes = polarParams->K>>3;
  int residue = polarParams->K&7;
  int numbits;

  if (residue>0) numbytes++;

  for (int byte=0; byte<numbytes; byte++) {
    if (byte<(polarParams->K>>3)) numbits=8;
    else numbits=residue;

    for (int val=0; val<256; val++) {
      polarParams->cprime_tab0[byte][val] = 0;
      polarParams->cprime_tab1[byte][val] = 0;

      for (int i=0; i<numbits; i++) {
        // flip bit endian of B bitstring
        ip=polarParams->deinterleaving_pattern[polarParams->K-1-((8*byte)+i)];
        AssertFatal(ip<128,"ip = %d\n",ip);
        bit_i=(val>>i)&1;

        if (ip<64) polarParams->cprime_tab0[byte][val] |= (((uint64_t)bit_i)<<ip);
        else       polarParams->cprime_tab1[byte][val] |= (((uint64_t)bit_i)<<(ip&63));
      }
    }
  }

  AssertFatal(polarParams->N==512 || polarParams->N==256 || polarParams->N==128,"N = %d, not done yet\n",polarParams->N);
  // build G bit vectors for information bit positions and convert the bit as bytes tables in nr_polar_kronecker_power_matrices.c to 64 bit packed vectors.
  // keep only rows of G which correspond to information/crc bits
  polarParams->G_N_tab = (uint64_t **)malloc((polarParams->K + polarParams->n_pc) * sizeof(int64_t *));
  int k=0;

  for (int i=0; i<polarParams->N; i++) {
    if (polarParams->information_bit_pattern[i] > 0) {
      polarParams->G_N_tab[k] = (uint64_t *)memalign(32,(polarParams->N/64)*sizeof(uint64_t));
      memset((void *)polarParams->G_N_tab[k],0,(polarParams->N/64)*sizeof(uint64_t));

      for (int j=0; j<polarParams->N; j++)
        polarParams->G_N_tab[k][j/64] |= ((uint64_t)polarParams->G_N[i][j])<<(j&63);

#ifdef DEBUG_POLAR_ENCODER
      printf("Bit %d Selecting row %d of G : ",k,i);

      for (int j=0; j<polarParams->N; j+=4) printf("%1x",polarParams->G_N[i][j]+(polarParams->G_N[i][j+1]*2)+(polarParams->G_N[i][j+2]*4)+(polarParams->G_N[i][j+3]*8));

      printf("\n");
#endif
      k++;
    }
  }

  // rate matching table
  int iplast=polarParams->rate_matching_pattern[0];
  int ccnt=0;
  int groupcnt=0;
#ifdef DEBUG_POLAR_ENCODER
  int firstingroup_out=0;
  int firstingroup_in=iplast;
#endif
  int mingroupsize = 1024;

  // compute minimum group size of rate-matching pattern
  for (int outpos=1; outpos<polarParams->encoderLength; outpos++) {
    ip=polarParams->rate_matching_pattern[outpos];
#ifdef DEBUG_POLAR_ENCODER
    printf("rm: outpos %d, inpos %d\n",outpos,ip);
#endif
    if ((ip - iplast) == 1) ccnt++;
    else {
      groupcnt++;
#ifdef DEBUG_POLAR_ENCODER
      printf("group %d (size %d): (%d:%d) => (%d:%d)\n",groupcnt,ccnt+1,
             firstingroup_in,firstingroup_in+ccnt,
             firstingroup_out,firstingroup_out+ccnt);
#endif

      if ((ccnt+1)<mingroupsize) mingroupsize=ccnt+1;

      ccnt=0;
#ifdef DEBUG_POLAR_ENCODER
      firstingroup_out=outpos;
      firstingroup_in=ip;
#endif
    }

    iplast=ip;
  }
  groupcnt++;
  if ((ccnt+1)<mingroupsize) mingroupsize=ccnt+1;
#ifdef DEBUG_POLAR_ENCODER
  printf("group %d (size %d): (%d:%d) => (%d:%d)\n",groupcnt,ccnt+1,
             firstingroup_in,firstingroup_in+ccnt,
             firstingroup_out,firstingroup_out+ccnt);
#endif
  AssertFatal(mingroupsize==4 || mingroupsize==8 || mingroupsize==16,"mingroupsize %d, needs to be handled\n",mingroupsize);
  polarParams->groupsize=mingroupsize;
  int shift=3;

  if (mingroupsize == 16) shift=4;
  else if (mingroupsize == 4) shift=2;

  polarParams->rm_tab=(int *)malloc(sizeof(int)*polarParams->encoderLength/mingroupsize);
  // rerun again to create groups
  int tcnt=0;

  for (int outpos=0; outpos<polarParams->encoderLength; outpos+=mingroupsize,tcnt++)
    polarParams->rm_tab[tcnt] = polarParams->rate_matching_pattern[outpos]>>shift;
}



decoder_node_t *add_nodes(int level, int first_leaf_index, t_nrPolar_params *polarParams) {

  int all_frozen_below = 1;
  int Nv = 1<<level;
  decoder_node_t *new_node = new_decoder_node(first_leaf_index, level);
#ifdef DEBUG_NEW_IMPL
  printf("New node %d order %d, level %d\n",polarParams->tree.num_nodes,Nv,level);
#endif
  polarParams->tree.num_nodes++;
  if (level==0) {
#ifdef DEBUG_NEW_IMPL
    printf("leaf %d (%s)\n", first_leaf_index, polarParams->information_bit_pattern[first_leaf_index]==1 ? "information or crc" : "frozen");
#endif
    new_node->leaf=1;
    new_node->all_frozen = polarParams->information_bit_pattern[first_leaf_index]==0 ? 1 : 0;
    return new_node; // this is a leaf node
  }

  for (int i=0;i<Nv;i++) {
    if (polarParams->information_bit_pattern[i+first_leaf_index]>0) {
    	  all_frozen_below=0;
        break;
    }
  }

  if (all_frozen_below==0)
	  new_node->left=add_nodes(level-1, first_leaf_index, polarParams);
  else {
#ifdef DEBUG_NEW_IMPL
    printf("aggregating frozen bits %d ... %d at level %d (%s)\n",first_leaf_index,first_leaf_index+Nv-1,level,((first_leaf_index/Nv)&1)==0?"left":"right");
#endif
    new_node->leaf=1;
    new_node->all_frozen=1;
  }
  if (all_frozen_below==0)
	  new_node->right=add_nodes(level-1,first_leaf_index+(Nv/2),polarParams);

#ifdef DEBUG_NEW_IMPL
  printf("new_node (%d): first_leaf_index %d, left %p, right %p\n",Nv,first_leaf_index,new_node->left,new_node->right);
#endif

  return(new_node);
}


void build_decoder_tree(t_nrPolar_params *polarParams)
{
  polarParams->tree.num_nodes=0;
  polarParams->tree.root = add_nodes(polarParams->n,0,polarParams);
#ifdef DEBUG_NEW_IMPL
  printf("root : left %p, right %p\n",polarParams->tree.root->left,polarParams->tree.root->right);
#endif
}

decoder_node_t *new_decoder_node(int first_leaf_index, int level) {

  decoder_node_t *node=(decoder_node_t *)malloc(sizeof(decoder_node_t));

  node->first_leaf_index=first_leaf_index;
  node->level=level;
  node->Nv = 1<<level;
  node->leaf = 0;
  node->left=(decoder_node_t *)NULL;
  node->right=(decoder_node_t *)NULL;
  node->all_frozen=0;
  node->alpha  = (int16_t*)malloc16(node->Nv*sizeof(int16_t));
  node->beta   = (int16_t*)malloc16(node->Nv*sizeof(int16_t));
  memset((void*)node->beta,-1,node->Nv*sizeof(int16_t));
  
  return(node);
}

void nr_polar_channel_interleaver_pattern(uint16_t *cip,
					  uint8_t I_BIL,
					  uint16_t E)
{
  if (I_BIL == 1) {
    uint16_t T=0, k;
    while( ((T/2)*(T+1)) < E ) T++;
    
    int16_t **v = malloc(T * sizeof(*v));
    for (int i = 0; i <= T-1; i++) v[i] = malloc((T-i) * sizeof(*(v[i])));
    
    k=0;
    for (int i = 0; i <= T-1; i++) {
      for (int j = 0; j <= (T-1)-i; j++) {
	if (k<E) {
	  v[i][j] = k;
	} else {
	  v[i][j] = (-1);
	}
	k++;
      }
    }
    
    k=0;
    for (int j = 0; j <= T-1; j++) {
      for (int i = 0; i <= (T-1)-j; i++) {
	if ( v[i][j] != (-1) ) {
	  cip[k]=v[i][j];
	  k++;
	}
      }
    }
    
    for (int i = 0; i <= T-1; i++) free(v[i]);
    free(v);
    
  } else {
    for (int i=0; i<=E-1; i++) cip[i]=i;
  }
}


void generic_polar_decoder(const t_nrPolar_params *pp,decoder_node_t *node) {
  

  // Apply F to left
  applyFtoleft(pp, node);
  
  // if left is not a leaf recurse down to the left
  if (node->left->leaf==0)
    generic_polar_decoder(pp, node->left);

  applyGtoright(pp, node);
  
  if (node->right->leaf==0) generic_polar_decoder(pp, node->right);

  computeBeta(pp, node);

} 

void applyFtoleft(const t_nrPolar_params *pp, decoder_node_t *node) {
  
  int16_t *alpha_v=node->alpha;
  int16_t *alpha_l=node->left->alpha;
  int16_t *betal = node->left->beta;
  int16_t a,b,absa,absb,maska,maskb,minabs;

#ifdef DEBUG_NEW_IMPL
  printf("applyFtoleft %d, Nv %d (level %d,node->left (leaf %d, AF %d))\n",node->first_leaf_index,node->Nv,node->level,node->left->leaf,node->left->all_frozen);


  for (int i=0;i<node->Nv;i++) printf("i%d (frozen %d): alpha_v[i] = %d\n",i,1-pp->information_bit_pattern[node->first_leaf_index+i],alpha_v[i]);
#endif

 
  if (node->left->all_frozen == 0) {
#if defined(__AVX2__)
    int avx2mod = (node->Nv/2)&15;
    if (avx2mod == 0) {
      __m256i a256,b256,absa256,absb256,minabs256;
      int avx2len = node->Nv/2/16;

      // printf("avx2len %d avx2mod %d\n",avx2len, avx2mod);
      for (int i=0;i<avx2len;i++) {
          
	a256       =((__m256i*)alpha_v)[i];
	b256       =((__m256i*)alpha_v)[i+avx2len];
	absa256    =_mm256_abs_epi16(a256);
	absb256    =_mm256_abs_epi16(b256);
	minabs256  =_mm256_min_epi16(absa256,absb256);
	((__m256i*)alpha_l)[i] =_mm256_sign_epi16(minabs256,_mm256_sign_epi16(a256,b256));
      }
    }
    else if (avx2mod == 8) {

      __m128i a128,b128,absa128,absb128,minabs128;
      a128       =*((__m128i*)alpha_v);
      b128       =((__m128i*)alpha_v)[1];
      absa128    =_mm_abs_epi16(a128);
      absb128    =_mm_abs_epi16(b128);
      minabs128  =_mm_min_epi16(absa128,absb128);
      *((__m128i*)alpha_l) =_mm_sign_epi16(minabs128,_mm_sign_epi16(a128,b128));
    }
    else if (avx2mod == 4) {
      __m64 a64,b64,absa64,absb64,minabs64;
      a64       =*((__m64*)alpha_v);
      b64       =((__m64*)alpha_v)[1];
      absa64    =_mm_abs_pi16(a64);
      absb64    =_mm_abs_pi16(b64);
      minabs64  =_mm_min_pi16(absa64,absb64);
      *((__m64*)alpha_l) =_mm_sign_pi16(minabs64,_mm_sign_pi16(a64,b64));
      
    }
    else
#else
    int sse4mod = (node->Nv/2)&7;
    int sse4len = node->Nv/2/8;
#if defined(__arm__) || defined(__aarch64__)
    int16x8_t signatimesb,comp1,comp2,negminabs128;
    int16x8_t zero=vdupq_n_s16(0);
#endif

    if (sse4mod == 0) {
      for (int i=0;i<sse4len;i++) {
	__m128i a128,b128,absa128,absb128,minabs128;
	int sse4len = node->Nv/2/8;
	
	a128       =*((__m128i*)alpha_v);
	b128       =((__m128i*)alpha_v)[1];
	absa128    =_mm_abs_epi16(a128);
	absb128    =_mm_abs_epi16(b128);
	minabs128  =_mm_min_epi16(absa128,absb128);
#if defined(__arm__) || defined(__aarch64__)
	// unfortunately no direct equivalent to _mm_sign_epi16
	signatimesb=vxorrq_s16(a128,b128);
	comp1=vcltq_s16(signatimesb,zero);
	comp2=vcgeq_s16(signatimesb,zero);
	negminabs128=vnegq_s16(minabs128);
	*((__m128i*)alpha_l) =vorrq_s16(vandq_s16(minabs128,comp0),vandq_s16(negminabs128,comp1));
#else
	*((__m128i*)alpha_l) =_mm_sign_epi16(minabs128,_mm_sign_epi16(a128,b128));
#endif
      }
    }
    else if (sse4mod == 4) {
      __m64 a64,b64,absa64,absb64,minabs64;
      a64       =*((__m64*)alpha_v);
      b64       =((__m64*)alpha_v)[1];
      absa64    =_mm_abs_pi16(a64);
      absb64    =_mm_abs_pi16(b64);
      minabs64  =_mm_min_pi16(absa64,absb64);
#if defined(__arm__) || defined(__aarch64__)
	AssertFatal(1==0,"Need to do this still for ARM\n");
#else
      *((__m64*)alpha_l) =_mm_sign_pi16(minabs64,_mm_sign_epi16(a64,b64));
#endif
    }

    else
#endif
    { // equivalent scalar code to above, activated only on non x86/ARM architectures
      
      for (int i=0;i<node->Nv/2;i++) {
        
    	  a=alpha_v[i];
    	  b=alpha_v[i+(node->Nv/2)];
    	  maska=a>>15;
    	  maskb=b>>15;
    	  absa=(a+maska)^maska;
    	  absb=(b+maskb)^maskb;
    	  minabs = absa<absb ? absa : absb;
    	  alpha_l[i] = (maska^maskb)==0 ? minabs : -minabs;
    	  //	printf("alphal[%d] %d (%d,%d)\n",i,alpha_l[i],a,b);
    	  }
    }
    if (node->Nv == 2) { // apply hard decision on left node
      betal[0] = (alpha_l[0]>0) ? -1 : 1;
#ifdef DEBUG_NEW_IMPL
      printf("betal[0] %d (%p)\n",betal[0],&betal[0]);
#endif
      pp->nr_polar_U[node->first_leaf_index] = (1+betal[0])>>1; 
#ifdef DEBUG_NEW_IMPL
      printf("Setting bit %d to %d (LLR %d)\n",node->first_leaf_index,(betal[0]+1)>>1,alpha_l[0]);
#endif
    }
  }
}

void applyGtoright(const t_nrPolar_params *pp,decoder_node_t *node) {

  int16_t *alpha_v=node->alpha;
  int16_t *alpha_r=node->right->alpha;
  int16_t *betal = node->left->beta;
  int16_t *betar = node->right->beta;

#ifdef DEBUG_NEW_IMPL
  printf("applyGtoright %d, Nv %d (level %d), (leaf %d, AF %d)\n",node->first_leaf_index,node->Nv,node->level,node->right->leaf,node->right->all_frozen);
#endif
  
  if (node->right->all_frozen == 0) {  
#if defined(__AVX2__) 
    int avx2mod = (node->Nv/2)&15;
    if (avx2mod == 0) {
      int avx2len = node->Nv/2/16;
      
      for (int i=0;i<avx2len;i++) {
	((__m256i *)alpha_r)[i] = 
	  _mm256_subs_epi16(((__m256i *)alpha_v)[i+avx2len],
			    _mm256_sign_epi16(((__m256i *)alpha_v)[i],
					      ((__m256i *)betal)[i]));	
      }
    }
    else if (avx2mod == 8) {
      ((__m128i *)alpha_r)[0] = _mm_subs_epi16(((__m128i *)alpha_v)[1],_mm_sign_epi16(((__m128i *)alpha_v)[0],((__m128i *)betal)[0]));	
    }
    else if (avx2mod == 4) {
      ((__m64 *)alpha_r)[0] = _mm_subs_pi16(((__m64 *)alpha_v)[1],_mm_sign_pi16(((__m64 *)alpha_v)[0],((__m64 *)betal)[0]));	
    }
    else
#else
    int sse4mod = (node->Nv/2)&7;

    if (sse4mod == 0) {
      int sse4len = node->Nv/2/8;
      
      for (int i=0;i<sse4len;i++) {
#if defined(__arm__) || defined(__aarch64__)
	((int16x8_t *)alpha_r)[0] = vsubq_s16(((int16x8_t *)alpha_v)[1],vmulq_epi16(((int16x8_t *)alpha_v)[0],((int16x8_t *)betal)[0]));
#else
	((__m128i *)alpha_r)[0] = _mm_subs_epi16(((__m128i *)alpha_v)[1],_mm_sign_epi16(((__m128i *)alpha_v)[0],((__m128i *)betal)[0]));
#endif	
      }
    }
    else if (sse4mod == 4) {
#if defined(__arm__) || defined(__aarch64__)
      ((int16x4_t *)alpha_r)[0] = vsub_s16(((int16x4_t *)alpha_v)[1],vmul_epi16(((int16x4_t *)alpha_v)[0],((int16x4_t *)betal)[0]));
#else
      ((__m64 *)alpha_r)[0] = _mm_subs_pi16(((__m64 *)alpha_v)[1],_mm_sign_pi16(((__64 *)alpha_v)[0],((__m64 *)betal)[0]));	
#endif
    }
    else 
#endif
      {// equivalent scalar code to above, activated only on non x86/ARM architectures or Nv=1,2
	for (int i=0;i<node->Nv/2;i++) {
	  alpha_r[i] = alpha_v[i+(node->Nv/2)] - (betal[i]*alpha_v[i]);
	}
      }
    if (node->Nv == 2) { // apply hard decision on right node
      betar[0] = (alpha_r[0]>0) ? -1 : 1;
      pp->nr_polar_U[node->first_leaf_index+1] = (1+betar[0])>>1;
#ifdef DEBUG_NEW_IMPL
      printf("Setting bit %d to %d (LLR %d)\n",node->first_leaf_index+1,(betar[0]+1)>>1,alpha_r[0]);
#endif
    } 
  }
}
unsigned int crcbit (unsigned char * inputptr,
		     int octetlen,
		     unsigned int poly)
{
  unsigned int i, crc = 0, c;

  while (octetlen-- > 0) {
    c = (*inputptr++) << 24;

    for (i = 8; i != 0; i--) {
      if ((1 << 31) & (c ^ crc))
        crc = (crc << 1) ^ poly;
      else
        crc <<= 1;

      c <<= 1;
    }
  }

  return crc;
}

int16_t all1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

void computeBeta(const t_nrPolar_params *pp,decoder_node_t *node) {

  int16_t *betav = node->beta;
  int16_t *betal = node->left->beta;
  int16_t *betar = node->right->beta;
#ifdef DEBUG_NEW_IMPL
  printf("Computing beta @ level %d first_leaf_index %d (all_frozen %d)\n",node->level,node->first_leaf_index,node->left->all_frozen);
#endif
  if (node->left->all_frozen==0) { // if left node is not aggregation of frozen bits
#if defined(__AVX2__) 
    int avx2mod = (node->Nv/2)&15;
    register __m256i allones=*((__m256i*)all1);
    if (avx2mod == 0) {
      int avx2len = node->Nv/2/16;
      for (int i=0;i<avx2len;i++) {
	((__m256i*)betav)[i] = _mm256_or_si256(_mm256_cmpeq_epi16(((__m256i*)betar)[i],
								  ((__m256i*)betal)[i]),allones);
      }
    }
    else if (avx2mod == 8) {
      ((__m128i*)betav)[0] = _mm_or_si128(_mm_cmpeq_epi16(((__m128i*)betar)[0],
							  ((__m128i*)betal)[0]),*((__m128i*)all1));
    }
    else if (avx2mod == 4) {
      ((__m64*)betav)[0] = _mm_or_si64(_mm_cmpeq_pi16(((__m64*)betar)[0],
						      ((__m64*)betal)[0]),*((__m64*)all1));
    }
    else
#else
    int avx2mod = (node->Nv/2)&15;

    if (ssr4mod == 0) {
      int ssr4len = node->Nv/2/8;
      register __m128i allones=*((__m128i*)all1);
      for (int i=0;i<sse4len;i++) {
      ((__m256i*)betav)[i] = _mm_or_si128(_mm_cmpeq_epi16(((__m128i*)betar)[i], ((__m128i*)betal)[i]),allones);
      }
    }
    else if (sse4mod == 4) {
      ((__m64*)betav)[0] = _mm_or_si64(_mm_cmpeq_pi16(((__m64*)betar)[0], ((__m64*)betal)[0]),*((__m64*)all1));
    }
    else
#endif
      {
	for (int i=0;i<node->Nv/2;i++) {
		betav[i] = (betal[i] != betar[i]) ? 1 : -1;
	}
      }
  }
  else memcpy((void*)&betav[0],betar,(node->Nv/2)*sizeof(int16_t));
  memcpy((void*)&betav[node->Nv/2],betar,(node->Nv/2)*sizeof(int16_t));
}
