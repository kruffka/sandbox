#include "dci_nr.h"

void exit_function(const char *file, const char *function, const int line, const char *s) {
//   int ru_id;

  if (s != NULL) {
    printf("%s:%d %s() Exiting OAI softmodem: %s\n",file,line, function, s);
  }

//   oai_exit = 1;

//   if (RC.ru == NULL)
//     exit(-1); // likely init not completed, prevent crash or hang, exit now...

//   for (ru_id=0; ru_id<RC.nb_RU; ru_id++) {
//     if (RC.ru[ru_id] && RC.ru[ru_id]->rfdevice.trx_end_func) {
//       RC.ru[ru_id]->rfdevice.trx_end_func(&RC.ru[ru_id]->rfdevice);
//       RC.ru[ru_id]->rfdevice.trx_end_func = NULL;
//     }

//     if (RC.ru[ru_id] && RC.ru[ru_id]->ifdevice.trx_end_func) {
//       RC.ru[ru_id]->ifdevice.trx_end_func(&RC.ru[ru_id]->ifdevice);
//       RC.ru[ru_id]->ifdevice.trx_end_func = NULL;
//     }
//   }

//   sleep(1); //allow lte-softmodem threads to exit first
  exit(1);
}

void write_arr(void *data, char *filename, int len) { 
    FILE *file; 
    file = fopen(filename, "w"); 
  
    if(file == NULL) { 
      exit_fun("file == NULL in dci_nr"); 
    } 
 
    // fprintf(file, "= ["); 
    for(int i = 0; i < len; i++){  
      fprintf(file, "(%d,%d)\n", ((short *)data)[2*i], ((short *)data)[2*i+1]); 
    } 
    // fprintf(file, "];"); 
    fclose(file); 
 
}

char nr_dci_format_string[8][30] = {
  "NR_DL_DCI_FORMAT_1_0",
  "NR_DL_DCI_FORMAT_1_1",
  "NR_DL_DCI_FORMAT_2_0",
  "NR_DL_DCI_FORMAT_2_1",
  "NR_DL_DCI_FORMAT_2_2",
  "NR_DL_DCI_FORMAT_2_3",
  "NR_UL_DCI_FORMAT_0_0",
  "NR_UL_DCI_FORMAT_0_1"};

// 0) L 4 CCE 0
// 1, 2

// 0) L 4 CCE 0
// 1) L 1 CCE 3
// 3, 4

// 0) L 1 CCE 3
// 1) L 4 CCE 0
// 5, 6


// 1) подставляю e из 4 декодирует с ситуацией 5,6
// 2) подставляю llr с ситуацией 5,6 не декодирует.
// e_rx неверно вычисляется по llr.

#define NR_PDCCH_DCI_DEBUG

#if 1
uint8_t nr_dci_decoding_procedure(PHY_VARS_NR_UE *ue,
                                  fapi_nr_dci_indication_t *dci_ind,
                                  fapi_nr_dl_config_dci_dl_pdu_rel15_t *rel15,
                                  NR_UE_PDCCH *pdcch_vars) {

  //int gNB_id = 0;
  int16_t tmp_e[16*108];
  rnti_t n_rnti;

  for (int j=0;j<rel15->number_of_candidates;j++) {
    int CCEind = rel15->CCE[j];
    int L = rel15->L[j];

    // Loop over possible DCI lengths
    for (int k = 0; k < rel15->num_dci_options; k++) {
      // skip this candidate if we've already found one with the
      // same rnti and format at a different aggregation level
      int dci_found=0;
      for (int ind=0;ind < dci_ind->number_of_dcis; ind++) {
        if (rel15->rnti== dci_ind->dci_list[ind].rnti &&
            rel15->dci_format_options[k]==dci_ind->dci_list[ind].dci_format) {
           dci_found=1;
           break;
        }
      }
      if (dci_found==1) continue;
      int dci_length = rel15->dci_length_options[k];
      uint64_t dci_estimation[2]= {0};
      const t_nrPolar_params *currentPtrDCI = nr_polar_params(NR_POLAR_DCI_MESSAGE_TYPE, dci_length, L, 1, &ue->polarList);
      

      printf("Trying DCI candidate %d of %d number of candidates, CCE %d (%d), L %d, length %d, format %s\n",
            j, rel15->number_of_candidates, CCEind, CCEind*9*6*2, L, dci_length,nr_dci_format_string[rel15->dci_format_options[k]]);


      // write_arr(&pdcch_vars->e_rx[0], "w/e_rx.m", 96*106); 
  
      int e_rx_idx = 0;
      for(int i = 0; i < j; i++) {
        e_rx_idx += 108*rel15->L[i];
      }
      printf("e_rx_idx = %d\n", e_rx_idx);
      nr_pdcch_unscrambling(&pdcch_vars->e_rx[e_rx_idx], rel15->coreset.scrambling_rnti, L*108, rel15->coreset.pdcch_dmrs_scrambling_id, tmp_e);

      // printf("CCEind = %d\n", CCEind*108);
      // nr_pdcch_unscrambling(&pdcch_vars->e_rx[CCEind*108], rel15->coreset.scrambling_rnti, L*108, rel15->coreset.pdcch_dmrs_scrambling_id, tmp_e);
      
      // write_arr(&tmp_e[0], "w/tmp_e.m", 1728); 

      uint16_t crc = polar_decoder_int16(tmp_e,
                                         dci_estimation,
                                         1,
                                         currentPtrDCI);


      n_rnti = rel15->rnti;

      printf("(%i.%i) Received dci indication (rnti %x, crc %x, dci format %s,n_CCE %d,payloadSize %d,payload %llx)\n",
           0, 0,n_rnti,crc,nr_dci_format_string[rel15->dci_format_options[k]],CCEind,dci_length,*(unsigned long long*)dci_estimation);

      // printf("(%i.%i) dci indication (rnti %x,dci format %s,n_CCE %d,payloadSize %d)\n",
      //       0, 0,n_rnti,nr_dci_format_string[rel15->dci_format_options[k]],CCEind,dci_length);

      if (crc == n_rnti) {
        printf("(%i.%i) Received dci indication (rnti %x,dci format %s,n_CCE %d,payloadSize %d,payload %llx)\n",
              0, 0,n_rnti,nr_dci_format_string[rel15->dci_format_options[k]],CCEind,dci_length,*(unsigned long long*)dci_estimation);

        for(int i = 0; i < dci_length; i++) {
          printf("%2d ", i);
        }
        printf("\n");
        for(int i = 0; i < dci_length; i++) {
        
          printf("%2d ", *(unsigned long long*)dci_estimation&((unsigned long long)pow(2,i)) ? 1 : 0);
          // if(!(i&3)) printf(" ");
        
        }
        printf("\ndci_estimation first bit %d\n", *(unsigned long long*)dci_estimation&(unsigned long long)pow(2,4*(dci_length/4)) ? 1 : 0);
        printf("bitwise dci est first bit %d\n", *(unsigned long long*)dci_estimation&(unsigned long long)1<<(4*(dci_length/4)) ? 1 : 0);
        printf("DCI DECODED!!!!\n");
      } else {
        printf("(%i.%i) Decoded crc %x does not match rnti %x for DCI format %d\n", 0, 0, crc, n_rnti, rel15->dci_format_options[k]);
      }
    }
  }

  return(0);
}

#endif


int32_t nr_pdcch_llr(int N_RB_DL, int32_t *rxdataF_comp,
                     int16_t *pdcch_llr, uint8_t symbol,uint32_t coreset_nbr_rb) {

  int16_t *rxF = (int16_t *) &rxdataF_comp[(symbol * coreset_nbr_rb * 12)];
  int32_t i;
  int16_t *pdcch_llrp;
  pdcch_llrp = &pdcch_llr[2 * symbol * coreset_nbr_rb * 9];

  if (!pdcch_llrp) {
    printf("pdcch_qpsk_llr: llr is null, symbol %d\n", symbol);
    return (-1);
  }

  // printf("llr logs: pdcch qpsk llr for symbol %d (pos %d), llr offset %ld\n",symbol,(symbol*N_RB_DL*12),pdcch_llrp-pdcch_llr);

  //for (i = 0; i < (frame_parms->N_RB_DL * ((symbol == 0) ? 16 : 24)); i++) {
  for (i = 0; i < (coreset_nbr_rb * ((symbol == 0) ? 18 : 18)); i++) {
    if (*rxF > 31)
      *pdcch_llrp = 31;
    else if (*rxF < -32)
      *pdcch_llrp = -32;
    else
      *pdcch_llrp = (*rxF);

    // printf("llr logs: rb=%d i=%d *rxF:%d => *pdcch_llrp:%d\n",i/18,i,*rxF,*pdcch_llrp);
    rxF++;
    pdcch_llrp++;
  }

  return (0);
}

void nr_pdcch_demapping_deinterleaving(uint32_t *llr,
                                       uint32_t *z,
                                       uint8_t coreset_time_dur,
                                       uint8_t start_symbol,
                                       uint32_t coreset_nbr_rb,
                                       uint8_t reg_bundle_size_L,
                                       uint8_t coreset_interleaver_size_R,
                                       uint8_t n_shift,
                                       uint8_t number_of_candidates,
                                       uint16_t *CCE,
                                       uint8_t *L) {  

  printf("coreset_time_dur %d start_symbol %d coreset_nbr_rb %d reg_bundle_size_L %d coreset_interleaver_size_R %d n_shift %d\n", coreset_time_dur, start_symbol, coreset_nbr_rb, reg_bundle_size_L, coreset_interleaver_size_R, n_shift);
  
  /*
   * This function will do demapping and deinterleaving from llr containing demodulated symbols
   * Demapping will regroup in REG and bundles
   * Deinterleaving will order the bundles
   *
   * In the following example we can see the process. The llr contains the demodulated IQs, but they are not ordered from REG 0,1,2,..
   * In e_rx (z) we will order the REG ids and group them into bundles.
   * Then we will put the bundles in the correct order as indicated in subclause 7.3.2.2
   *
   llr --------------------------> e_rx (z) ----> e_rx (z)
   |   ...
   |   ...
   |   REG 26
   symbol 2    |   ...
   |   ...
   |   REG 5
   |   REG 2

   |   ...
   |   ...
   |   REG 25
   symbol 1    |   ...
   |   ...
   |   REG 4
   |   REG 1

   |   ...
   |   ...                           ...              ...
   |   REG 24 (bundle 7)             ...              ...
   symbol 0    |   ...                           bundle 3         bundle 6
   |   ...                           bundle 2         bundle 1
   |   REG 3                         bundle 1         bundle 7
   |   REG 0  (bundle 0)             bundle 0         bundle 0

  */
  int c = 0, r = 0;
  uint16_t bundle_j = 0, f_bundle_j = 0, f_reg = 0;
  uint32_t coreset_C = 0;
  uint16_t index_z, index_llr;
  int coreset_interleaved = 0;

  printf("reg_bundle_size_L %d\n", reg_bundle_size_L);
  if (reg_bundle_size_L != 0) { // interleaving will be done only if reg_bundle_size_L != 0
    // AssertFatal(1==0, "deinterleaving from llr containing demodulated symbols in %s is not really implemented", __FUNCTION__);
    coreset_interleaved = 1;
    coreset_C = (uint32_t) (coreset_nbr_rb / (coreset_interleaver_size_R * reg_bundle_size_L));
  } else {
    reg_bundle_size_L = 6; // true
  }

  int f_bundle_j_list[(2*NR_MAX_PDCCH_AGG_LEVEL) - 1] = {};

  for (int reg = 0; reg < coreset_nbr_rb; reg++) { // 0..24
    if ((reg % reg_bundle_size_L) == 0) {
      if (r == coreset_interleaver_size_R) {
        r = 0;
        c++;
      }
      // printf("c %d r %d\n", c, r);

      bundle_j = (c * coreset_interleaver_size_R) + r; // bundle_j = r (0, 1, 2, 3)
      f_bundle_j = ((r * coreset_C) + c + n_shift) % (coreset_nbr_rb / reg_bundle_size_L); // f_bundle_j = c % 6 = 1

      if (coreset_interleaved == 0) f_bundle_j = bundle_j; // true (0, 1, 2, 3)

      f_bundle_j_list[reg / 6] = f_bundle_j; // [0, 1, 2, 3]

    }
    if ((reg % reg_bundle_size_L) == 0) r++; // (0, 1, 2, 3)
  }

//   // Get cce_list indices by reg_idx in ascending order
//   int f_bundle_j_list_id = 0;
//   int f_bundle_j_list_ord[(2*NR_MAX_PDCCH_AGG_LEVEL)-1] = {};
//   for (int c_id = 0; c_id < number_of_candidates; c_id++ ) { // 0..2
//     f_bundle_j_list_id = CCE[c_id]; // [3, 0]
//     for (int p = 0; p < NR_MAX_PDCCH_AGG_LEVEL; p++) { // 0..16
//       for (int p2 = CCE[c_id]; p2 < CCE[c_id] + L[c_id]; p2++) { // 3 < 4; 0 .. 4
//         AssertFatal(p2<2*NR_MAX_PDCCH_AGG_LEVEL,"number_of_candidates %d : p2 %d,  CCE[%d] %d, L[%d] %d\n",number_of_candidates,p2,c_id,CCE[c_id],c_id,L[c_id]);
//         if (f_bundle_j_list[p2] == p) { //  3 == 0..16 
//           AssertFatal(f_bundle_j_list_id < 2*NR_MAX_PDCCH_AGG_LEVEL,"f_bundle_j_list_id %d\n",f_bundle_j_list_id);
//           f_bundle_j_list_ord[f_bundle_j_list_id] = p; // [3] = 3; [0] = 0; [1] = 1; [2] = 2; [3] = 3;
//           f_bundle_j_list_id++; // 4
//           break;
//         }
//       }
//     }
//   }

  
//     int rb = 0;
//   for (int c_id = 0; c_id < number_of_candidates; c_id++ ) { // [0..2)
//     for (int symbol_idx = start_symbol; symbol_idx < start_symbol+coreset_time_dur; symbol_idx++) { // [0..2)
//       for (int cce_count = CCE[c_id/coreset_time_dur]+c_id%coreset_time_dur; cce_count < CCE[c_id/coreset_time_dur]+c_id%coreset_time_dur+L[c_id]; cce_count += coreset_time_dur) { // cce_count 0 or 1; < 1 or < 5 
//       // for (int cce_count = CCE[c_id]; cce_count < CCE[c_id]+L[c_id]; cce_count += coreset_time_dur) { // cce_count 0 or 1; < 1 or < 5 
//         for (int reg_in_cce_idx = 0; reg_in_cce_idx < NR_NB_REG_PER_CCE; reg_in_cce_idx++) { // 0..6

//           f_reg = (f_bundle_j_list_ord[cce_count] * reg_bundle_size_L) + reg_in_cce_idx;

//           index_z = 9 * rb;
//           index_llr = (uint16_t) (f_reg + symbol_idx * coreset_nbr_rb) * 9;

//           for (int i = 0; i < 9; i++) {
//             z[index_z + i] = llr[index_llr + i];
// #ifdef NR_PDCCH_DCI_DEBUG

//             printf("[cce_count=%d,reg_in_cce_idx=%d,bundle_j=%d,symbol_idx=%d,candidate=%d] z[%d]=(%d,%d) <-> \t[f_reg=%d,fbundle_j=%d] llr[%d]=(%d,%d) cce_idx %d\n",
//                   cce_count,reg_in_cce_idx,bundle_j,symbol_idx,c_id,(index_z + i),*(int16_t *) &z[index_z + i],*(1 + (int16_t *) &z[index_z + i]),
//                    f_reg,f_bundle_j,(index_llr + i),*(int16_t *) &llr[index_llr + i], *(1 + (int16_t *) &llr[index_llr + i]), (index_llr + i)/(9*3));
// #endif
//           }
//           rb++;
//         }
//       }
//     }
//   }

  
  int reg_in_cce_symbol = NR_NB_REG_PER_CCE/coreset_time_dur;

  int rb = 0;
  for (int c_id = 0; c_id < number_of_candidates; c_id++) {
    for (int symbol_idx = start_symbol; symbol_idx < start_symbol+coreset_time_dur; symbol_idx++) {
      for (int cce_count = CCE[c_id]; cce_count < CCE[c_id]+L[c_id]; cce_count++) {
        for (int reg_in_cce_idx = 0; reg_in_cce_idx < reg_in_cce_symbol; reg_in_cce_idx++) {

          f_reg = cce_count*reg_in_cce_symbol + reg_in_cce_idx;
          
          index_z = 9 * rb;
          index_llr = (f_reg + symbol_idx*coreset_nbr_rb) * 9;

          for (int i = 0; i < 9; i++) {
            z[index_z + i] = llr[index_llr + i];
          
            #ifdef NR_PDCCH_DCI_DEBUG

              printf("[cce_count=%d,reg_in_cce_idx=%d,bundle_j=%d,symbol_idx=%d,candidate=%d] z[%d]=(%d,%d) <-> \t[f_reg=%d,fbundle_j=%d] llr[%d]=(%d,%d) cce_idx %d\n",
                    cce_count,reg_in_cce_idx,bundle_j,symbol_idx,c_id,(index_z + i),*(int16_t *) &z[index_z + i],*(1 + (int16_t *) &z[index_z + i]),
                    f_reg,f_bundle_j,(index_llr + i),*(int16_t *) &llr[index_llr + i], *(1 + (int16_t *) &llr[index_llr + i]), (index_llr + i)/(9*3));

            #endif

          }

          rb++;
        }
      }
    }
  }


}

void rxdataF_comp_read(void *data, char *filename){ 
  
    FILE *file; 
    file = fopen(filename, "r"); 
    printf("filename = %s\n", filename);
  
    if(file == NULL) { 
      exit_fun("file == NULL in dci_nr"); 
    } 
    fscanf(file, "rxdataF_comp = ["); 
    int32_t re, im; 
    for(int i = 0; i < 2*12*106; i++){  
      fscanf(file, "%d + j*(%d)\n", &re, &im); 
 
      ((short *)data)[2*i] = (short)(re); 
      ((short *)data)[2*i+1] = (short)(im); 
          
    } 
    fscanf(file, "];"); 
    fclose(file); 

    for(int i = 0; i < 10; i++) {
      printf("%d + j*(%d)\n", 
        ((short *)data)[2*i], 
        ((short *)data)[2*i+1] 
       );
    }
}

int main() {

    crcTableInit();

    NR_UE_PDCCH *pdcch_vars = (NR_UE_PDCCH *)malloc(sizeof(NR_UE_PDCCH));
    fapi_nr_dl_config_dci_dl_pdu_rel15_t *rel15 = (fapi_nr_dl_config_dci_dl_pdu_rel15_t *)malloc(sizeof(fapi_nr_dl_config_dci_dl_pdu_rel15_t));
    PHY_VARS_NR_UE *ue = (PHY_VARS_NR_UE *)malloc(sizeof(PHY_VARS_NR_UE));
    fapi_nr_dci_indication_t *dci_ind = (fapi_nr_dci_indication_t *)malloc(sizeof(fapi_nr_dci_indication_t));

    // ue->polarList = (t_nrPolar_params *)malloc(sizeof(t_nrPolar_params));

    pdcch_vars->llr = (int16_t *)malloc(sizeof(int16_t)*48*106);
    pdcch_vars->e_rx = (int16_t *)malloc(sizeof(int16_t)*96*106);
    pdcch_vars->rxdataF_comp = (int32_t *)malloc(sizeof(int32_t)*168*106);

    // memset(&pdcch_vars->llr[0], 0, 48*106*sizeof(int16_t)); 

    // FILE *file = fopen("/home/usrpuser/test/nr_dci_decoding/pdcch_llr_frame329slot1.m", "r"); 
    // if(file == NULL) { 
    //     exit_fun("probably need to mkdir decoding in /tmp"); 
    // } 
 
    // printf("reading pdcch_llr_frame329slot1\n"); 
    // fscanf(file, "llr = ["); 
    // for(int i = 0; i < 48*106; i++) { 
    //     fscanf(file, "%d, ", &pdcch_vars->llr[i]); 
    // } 
    // fscanf(file, "];"); 
 
    // // for(int i = 0; i < 5; i++) { 
    // //     printf("%d\n", pdcch_vars->llr[i]); 
    // // } 
 
    // fclose(file);


    // 
    // Frame, slot from file
    //
    int frame = 545; //545
    int slot = 2; //5

    char filename[100];
    sprintf(filename, "/home/usrpuser/test/nr_dci_decoding/rx/rxdataF_comp_frame%dslot%d.m", frame, slot);
    // sprintf(filename, "/home/usrpuser/test/nr_dci_decoding/rx_3/rxdataF_comp.m", frame, slot);


    rxdataF_comp_read(&pdcch_vars->rxdataF_comp[0], filename);


    //
    //  Candidates, L, CCE, options5966
    // 
    rel15->num_dci_options = 2;
    rel15->dci_format_options[0] = 0; // NR_DL_DCI_1_0
    rel15->dci_format_options[1] = 6; // NR_UL_DCI_0_0

    rel15->number_of_candidates = 1;
    rel15->L[0] = 1;
    rel15->CCE[0] = 2;
    // rel15->L[1] = 4;
    // rel15->CCE[1] = 4;
    // rel15->L[2] = 4;
    // rel15->CCE[2] = 0;

    rel15->dci_length_options[0] = 41;
    rel15->dci_length_options[1] = 41;



    int n_rb = 24;
    int N_RB_DL = 106;

    for(int s = 0; s < 2; s++) { // 2 pdcch symbols
      nr_pdcch_llr(N_RB_DL,
                    pdcch_vars->rxdataF_comp,
                    pdcch_vars->llr,
                    s,
                    n_rb);
    }

    {
    
    FILE *file = fopen("/home/usrpuser/test/nr_dci_decoding/w/pdcch_llr.m", "w"); 
    if(file == NULL) { 
        exit_fun("error opening file"); 
    }
 
    printf("writing pdcch_llr\n"); 
    fprintf(file, "llr = ["); 
    for(int i = 0; i < 48*106; i++) { 
        fprintf(file, "%d, ", pdcch_vars->llr[i]); 
    } 
    fprintf(file, "];"); 
 
    // for(int i = 0; i < 5; i++) { 
    //     printf("%d\n", pdcch_vars->llr[i]); 
    // } 
 
    fclose(file);
    }

    // write_arr(&pdcch_vars->llr[0], "w/pdcch_llr.m", 48*106); 

    
    rel15->rnti = 0x46; 
    rel15->coreset.scrambling_rnti = 0;
    rel15->coreset.pdcch_dmrs_scrambling_id = 0;
    // rel15->coreset.CceRegMappingType = 1;
    // rel15->coreset.CoreSetType = 0;
    // rel15->coreset.precoder_granularity = 0;
    // rel15->coreset.tci_present_in_dci = 2;
    // rel15->coreset.tci_state_pdcch = 5;
    // memset(&pdcch_vars->e_rx, 0, 96*106*sizeof(int16_t)); 
    // dur 2, startsymbolindex 0, n_rb 24, regbundlesize 0 interleaversize 0 shiftindex 0 noc 2
    
    rel15->coreset.duration = 2;
    rel15->coreset.StartSymbolIndex = 0;
    rel15->coreset.RegBundleSize = 0;
    rel15->coreset.InterleaverSize = 0;
    rel15->coreset.ShiftIndex = 0;
    



    nr_pdcch_demapping_deinterleaving((uint32_t *) pdcch_vars->llr,
                                    (uint32_t *) pdcch_vars->e_rx,
                                    rel15->coreset.duration,
                                    rel15->coreset.StartSymbolIndex,
                                    n_rb,
                                    rel15->coreset.RegBundleSize,
                                    rel15->coreset.InterleaverSize,
                                    rel15->coreset.ShiftIndex,
                                    rel15->number_of_candidates,
                                    rel15->CCE,
                                    rel15->L);

    
    // printf("pdcch_llr[%d] = %d %d\n", 432, pdcch_vars->llr[862], pdcch_vars->llr[863]);

    // for(int i = 0, index_e = 0; i < 864; i++) {
    //   if(i >= 162 && i < 216) { // 0-54
    //     pdcch_vars->e_rx[index_e++] = pdcch_vars->llr[i];
    //     // printf("pdcch_vars->llr[%d] = %d\n", i, pdcch_vars->llr[i]); 
    //   } else if (i >= 594 && i < 648) { //162-216
    //     pdcch_vars->e_rx[index_e++] = pdcch_vars->llr[i]; //54
    //     // printf("pdcch_vars->llr[%d] = %d\n", i, pdcch_vars->llr[i]); 
    //   } else if (i >= 594 && i < 702) { // 216-270
    //     pdcch_vars->e_rx[index_e++] = 0;//pdcch_vars->llr[i]; // 108
    //   //   // printf("pdcch_vars->llr[%d] = %d\n", 648-54*2+i, pdcch_vars->llr[648-54*2+i]); 
    //   } else if (i >= 756 && i < 864) { //389-432
    //     pdcch_vars->e_rx[index_e++] = 0;//pdcch_vars->llr[i]; // 162
    //     // printf("pdcch_vars->llr[%d] = %d\n", 378+i, pdcch_vars->llr[378+i]); 
    //   }
    // }


    // for(int i = 108; i < 108+54; i++) {
    //   printf("llr [%d] = %d\n", i, pdcch_vars->llr[i]);
    // }
  //   printf("\n");
  // for(int i = 108; i < 108+20; i++) {
  //   printf("llr[%d] = %d %p\n", i, pdcch_vars->llr[i], &pdcch_vars->llr[i]);
  // }
    write_arr(&pdcch_vars->e_rx[0], "w/e_rx.m", 96*106); 


    for(int i = 0; i < rel15->number_of_candidates; i++) {
      int e_rx_idx = 0;
      for(int j = 0; j < i; j++) {
        e_rx_idx += 108*rel15->L[j]; //54*2
      }

      printf("i [%d] e_rx_idx %d\n", i, e_rx_idx);

  }
  

    printf("entering nr_dci_decoding\n");
    nr_dci_decoding_procedure(ue, dci_ind, rel15, pdcch_vars);
    printf("leaving nr_dci_decoding\n");


    free(pdcch_vars->rxdataF_comp);
    free(pdcch_vars->e_rx);
    free(pdcch_vars->llr);
    // free(ue->polarList);
    free(pdcch_vars);
    free(dci_ind);
    free(rel15);

    return 0;
}