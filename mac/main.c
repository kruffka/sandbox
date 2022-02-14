#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static NR_UE_MAC_INST_t *nr_ue_mac_inst; 
typedef uint16_t module_id_t;
#define NR_MAX_HARQ_PROCESSES 16
#define NB_NR_UE_MAC_INST 1

NR_UE_MAC_INST_t *get_mac_inst(module_id_t module_id){
    return &nr_ue_mac_inst[(int)module_id];
}


NR_UE_MAC_INST_t * nr_l2_init_ue(NR_UE_RRC_INST_t* rrc_inst) {

    nr_ue_mac_inst = (NR_UE_MAC_INST_t *)calloc(sizeof(NR_UE_MAC_INST_t),NB_NR_UE_MAC_INST);
    
    for (int j=0;j<NB_NR_UE_MAC_INST;j++)
	    for (int i=0;i<NR_MAX_HARQ_PROCESSES;i++) nr_ue_mac_inst[j].first_ul_tx[i]=1;

    printf("nr_ue_mac_inst[j].first_ul_tx[i] %d\n", nr_ue_mac_inst[0].first_ul_tx[0]);

    // if (rrc_inst && rrc_inst->scell_group_config) {
    //     printf("rrc_inst\n");

    // } else {
    //     printf("return 0\n");
    //     exit(0);
    // }


    return nr_ue_mac_inst;

}

void foo() {
    NR_UE_MAC_INST_t *mac_inst = get_mac_inst(0);;
    printf("mac_inmst = %d\n", mac_inst->test);
    
    mac_inst->test = 3;

}


int get_k2(int tda_id) {
    uint8_t mu_pusch = 1;
    uint8_t j = (mu_pusch==3)?3:(mu_pusch==2)?2:1;

    uint8_t table[2][3] = {
        {j, 0, 1},
        {j+1, 1, 2}
    };

    printf("k2 = %d\n", table[tda_id][0]);

}

int main(void) {

    get_k2(1);

    NR_UE_MAC_INST_t *mac_inst = nr_l2_init_ue(NULL);
    
    mac_inst->ULbwp[0] = (NR_BWP_Uplink_t *)malloc(sizeof(NR_BWP_Uplink_t));
    NR_BWP_Uplink_t *ubwp = mac_inst->ULbwp[0];
    ubwp->bwp_Id = 10;
    printf("ubwp->bwp_Id = %d\n", ubwp->bwp_Id);

    // ubwp->bwp_Id = 0;
    mac_inst->test = 0;
    printf("mac = %d\n", mac_inst->test);
    mac_inst->test = 5;

    foo();

    printf("4444 mac = %d\n", mac_inst->test);


    free(mac_inst);
}