
typedef __SIZE_TYPE__ size_t;
typedef size_t ber_tlv_len_t;
typedef long	 NR_BWP_Id_t;
typedef long NR_CellGroupId_t;

/* NR_CellGroupConfig */
typedef struct NR_CellGroupConfig {
	NR_CellGroupId_t	 cellGroupId;

} NR_CellGroupConfig_t;

typedef struct NR_UE_RRC_INST_s {
    NR_CellGroupConfig_t   *scell_group_config;

} NR_UE_RRC_INST_t;


typedef struct asn_struct_ctx_s {
	short phase;		/* Decoding phase */
	short step;		/* Elementary step of a phase */
	int context;		/* Other context information */
	void *ptr;		/* Decoder-specific stuff (stack elements) */
	ber_tlv_len_t left;	/* Number of bytes left, -1 for indefinite */
} asn_struct_ctx_t;

typedef struct NR_BWP_Uplink {
	NR_BWP_Id_t	 bwp_Id;
	struct NR_BWP_UplinkCommon	*bwp_Common;	/* OPTIONAL */
	struct NR_BWP_UplinkDedicated	*bwp_Dedicated;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NR_BWP_Uplink_t;

#define MAX_NUM_BWP 2


/*!\brief Top level UE MAC structure */
typedef struct {

    int test;
    int first_ul_tx[16];
    NR_BWP_Uplink_t                 *ULbwp[MAX_NUM_BWP];

} NR_UE_MAC_INST_t;



