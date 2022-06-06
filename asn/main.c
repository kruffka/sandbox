#include "asn1/MIB.h"
#include "asn1/BCCH-BCH-MessageType.h"
// #include "asn1/BCCH-BCH-Message.h"
#include "asn1/xer_encoder.h"
#include "asn1/FrequencyInfoDL-SIB.h"

int main() {


    BCCH_BCH_MessageType_t bcch = {0};
    bcch.present = BCCH_BCH_MessageType_PR_mib;
    bcch.choice.mib.subCarrierSpacingCommon = 0;
    bcch.choice.mib.systemFrameNumber.buf = CALLOC(1, sizeof(int16_t));
    bcch.choice.mib.systemFrameNumber.buf[0] = 0x17;
    bcch.choice.mib.systemFrameNumber.bits_unused = 0;
    bcch.choice.mib.systemFrameNumber.buf[1] = 0x17;
    printf("bcch.choice.mib.systemFrameNumber.size %ld\n", bcch.choice.mib.systemFrameNumber.size);
    // bcch.choice.mib.systemFrameNumber.size = 2*sizeof(int16_t);
    bcch.choice.mib.cellBarred = 0;
    bcch.choice.mib.dmrs_TypeA_Position = 0;
    bcch.choice.mib.intraFreqReselection = 0;
    bcch.choice.mib.pdcch_ConfigSIB1 = 0;
    bcch.choice.mib.ssb_SubcarrierOffset = 0;
    // bcch.choice.mib.spare.buf = CALLOC(1, sizeof(int16_t));
    // bcch.choice.mib.spare.buf[0] = 11;
    // bcch.choice.mib.spare.buf[1] = 0;
    // bcch.choice.mib.spare.size = 1*sizeof(int16_t);
    // bcch.choice.mib.spare.bits_unused = 0;
    // printf("scs = %ld\n", bcch.choice.mib.subCarrierSpacingCommon);


    // xer_fprint(stdout, &asn_DEF_BCCH_BCH_MessageType, (void *)&bcch);

    printf("\n\n");
    ssize_t bufsize = 512;
    uint8_t buffer[bufsize];
    // asn_enc_rval_t enc_rval = uper_encode_to_buffer(&asn_DEF_BCCH_BCH_MessageType, NULL, (void *)&bcch, buffer, bufsize);

    // asn_enc_rval_t dec_rval = uper_decode(&asn_DEF_BCCH_BCH_MessageType, NULL, &bcch, buffer, bufsize);

    // if(enc_rval.encoded == -1) {
    //     printf("[UE AssertFatal] ASN1 message encoding failed (%s, %lu)!\n", enc_rval.failed_type->name, enc_rval.encoded);

    //     SEQUENCE_free(&asn_DEF_BCCH_BCH_MessageType, &bcch, ASFM_FREE_UNDERLYING_AND_RESET);
    //     return -1;
    // } else {
    //     printf("UE SUCCESS!\n");
    // }

    FrequencyInfoDL_SIB_t sib1;
    sib1.offsetToPointA = 0;
sib1.scs_SpecificCarrierList.list.count = 1;
    sib1.scs_SpecificCarrierList.list.array = CALLOC(1, sizeof(SCS_SpecificCarrier_t **));
    sib1.scs_SpecificCarrierList.list.array[0] = CALLOC(1, 1*sizeof(SCS_SpecificCarrier_t *));

        // sib1.scs_SpecificCarrierList.list.count = 0;
        // sib1.scs_SpecificCarrierList.list.size = 55;

    sib1.scs_SpecificCarrierList.list.array[0]->carrierBandwidth = 50;
    sib1.scs_SpecificCarrierList.list.array[0]->subcarrierSpacing = 0;
    sib1.scs_SpecificCarrierList.list.array[0]->offsetToCarrier = 0;

    

    xer_fprint(stdout, &asn_DEF_FrequencyInfoDL_SIB, (void *)&sib1);

    printf("\n\n");

    asn_enc_rval_t enc_rval = uper_encode_to_buffer(&asn_DEF_FrequencyInfoDL_SIB, NULL, (void *)&sib1, buffer, bufsize);

    // asn_enc_rval_t dec_rval = uper_decode(&asn_DEF_BCCH_BCH_MessageType, NULL, &bcch, buffer, bufsize);

    if(enc_rval.encoded == -1) {
        printf("[UE AssertFatal] ASN1 message encoding failed (%s, %lu)!\n", enc_rval.failed_type->name, enc_rval.encoded);

        SEQUENCE_free(&asn_DEF_BCCH_BCH_MessageType, &bcch, ASFM_FREE_UNDERLYING_AND_RESET);
        return -1;
    } else {
        printf("UE SUCCESS!\n");
    }

    free(sib1.scs_SpecificCarrierList.list.array[0]);
    free(sib1.scs_SpecificCarrierList.list.array);
}