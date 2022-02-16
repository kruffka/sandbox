#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <mmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>

#ifndef __AVX2__
  #define __AVX2__
#endif 

#define CRC24_A 0
#define CRC24_B 1
#define CRC16 2
#define CRC8 3
#define NR_POLAR_DCI_MESSAGE_TYPE 1 //int8_t
#define NR_POLAR_DCI_CRC_PARITY_BITS 24
#define NR_POLAR_DCI_CRC_ERROR_CORRECTION_BITS 3

//Sec. 7.3.3: Channel Coding
#define NR_POLAR_DCI_N_MAX 9   //uint8_t
#define NR_POLAR_DCI_I_IL 1    //uint8_t
#define NR_POLAR_DCI_I_SEG 0   //uint8_t
#define NR_POLAR_DCI_N_PC 0    //uint8_t
#define NR_POLAR_DCI_N_PC_WM 0 //uint8_t

//Sec. 7.3.4: Rate Matching
#define NR_POLAR_DCI_I_BIL 0 //uint8_t

#define NR_POLAR_PBCH_AGGREGATION_LEVEL 0 //uint8_t
#define NR_POLAR_PBCH_MESSAGE_TYPE 0      //int8_t
#define NR_POLAR_PBCH_PAYLOAD_BITS 32     //uint16_t
#define NR_POLAR_PBCH_CRC_PARITY_BITS 24
#define NR_POLAR_PBCH_CRC_ERROR_CORRECTION_BITS 3
//Assumed 3 by 3GPP when NR_POLAR_PBCH_L>8 to meet false alarm rate requirements.

//Sec. 7.1.4: Channel Coding
#define NR_POLAR_PBCH_N_MAX 9   //uint8_t
#define NR_POLAR_PBCH_I_IL 1    //uint8_t
#define NR_POLAR_PBCH_I_SEG 0   //uint8_t
#define NR_POLAR_PBCH_N_PC 0    //uint8_t
#define NR_POLAR_PBCH_N_PC_WM 0 //uint8_t
//#define NR_POLAR_PBCH_N 512     //uint16_t

//Sec. 7.1.5: Rate Matching
#define NR_POLAR_PBCH_I_BIL 0 //uint8_t
#define NR_POLAR_PBCH_E 864   //uint16_t
#define NR_POLAR_PBCH_E_DWORD 27 // NR_POLAR_PBCH_E/32


#define NR_POLAR_UCI_PUCCH_MESSAGE_TYPE 2 //int8_t
#define NR_POLAR_PUCCH_CRC_ERROR_CORRECTION_BITS 3
#define NR_POLAR_PUCCH_PAYLOAD_BITS 32
#define NR_POLAR_PUCCH_E 32

//Ref. 38-212, Section 6.3.1.2.1
#define NR_POLAR_PUCCH_CRC_PARITY_BITS_SHORT 6
#define NR_POLAR_PUCCH_CRC_PARITY_BITS_LONG 11

#define NR_POLAR_PUCCH_I_SEG_LONG 1
#define NR_POLAR_PUCCH_I_SEG_SHORT 0

//Ref. 38-212, Section 6.3.1.3.1
#define NR_POLAR_PUCCH_N_MAX 10
#define NR_POLAR_PUCCH_I_IL 0
#define NR_POLAR_PUCCH_N_PC_SHORT 3
#define NR_POLAR_PUCCH_N_PC_LONG 0
#define NR_POLAR_PUCCH_N_PC_WM_LONG 0
#define NR_POLAR_PUCCH_N_PC_WM_SHORT 1

//Ref. 38-212, Section 6.3.1.4.1
#define NR_POLAR_PUCCH_I_BIL 1

#define malloc16(x) memalign(16,x+16)
#define NR_MAX_PDCCH_AGG_LEVEL 16
#define NR_NB_REG_PER_CCE 6

#define NR_POLAR_DECODER_LISTSIZE 8 //uint8_t

#define NR_POLAR_AGGREGATION_LEVEL_1_PRIME 149 //uint16_t
#define NR_POLAR_AGGREGATION_LEVEL_2_PRIME 151 //uint16_t
#define NR_POLAR_AGGREGATION_LEVEL_4_PRIME 157 //uint16_t
#define NR_POLAR_AGGREGATION_LEVEL_8_PRIME 163 //uint16_t
#define NR_POLAR_AGGREGATION_LEVEL_16_PRIME 167 //uint16_t


static const uint16_t Q_0_Nminus1_1[2] = { 0, 1 };
static const uint16_t Q_0_Nminus1_2[4] = { 0, 1, 2, 3 };
static const uint16_t Q_0_Nminus1_3[8] = { 0, 1, 2, 4, 3, 5, 6, 7 };
static const uint16_t Q_0_Nminus1_4[16] = { 0, 1, 2, 4, 8, 3, 5, 9, 6, 10, 12, 7, 11, 13, 14, 15 };
static const uint16_t Q_0_Nminus1_5[32] = { 0, 1, 2, 4, 8, 16, 3, 5, 9, 6, 17, 10, 18, 12, 20, 24, 7, 11, 19, 13, 14, 21, 26, 25, 22, 28, 15, 23, 27, 29, 30, 31 };
static const uint16_t Q_0_Nminus1_6[64] = { 0, 1, 2, 4, 8, 16, 32, 3, 5, 9, 6, 17, 10, 18, 12, 33, 20, 34, 24, 36, 7, 11, 40, 19, 13, 48, 14, 21, 35, 26, 37, 25, 22, 38, 41, 28, 42, 49, 44, 50, 15, 52, 23, 56, 27, 39, 29, 43, 30, 45, 51, 46, 53, 54, 57, 58, 60, 31, 47, 55, 59, 61, 62, 63 };
static const uint16_t Q_0_Nminus1_7[128] = { 0, 1, 2, 4, 8, 16, 32, 3, 5, 64, 9, 6, 17, 10, 18, 12, 33, 65, 20, 34, 24, 36, 7, 66, 11, 40, 68, 19, 13, 48, 14, 72, 21, 35, 26, 80, 37, 25, 22, 38, 96, 67, 41, 28, 69, 42, 49, 74, 70, 44, 81, 50, 73, 15, 52, 23, 76, 82, 56, 27, 97, 39, 84, 29, 43, 98, 88, 30, 71, 45, 100, 51, 46, 75, 104, 53, 77, 54, 83, 57, 112, 78, 85, 58, 99, 86, 60, 89, 101, 31, 90, 102, 105, 92, 47, 106, 55, 113, 79, 108, 59, 114, 87, 116, 61, 91, 120, 62, 103, 93, 107, 94, 109, 115, 110, 117, 118, 121, 122, 63, 124, 95, 111, 119, 123, 125, 126, 127 };
static const uint16_t Q_0_Nminus1_8[256] = { 0, 1, 2, 4, 8, 16, 32, 3, 5, 64, 9, 6, 17, 10, 18, 128, 12, 33, 65, 20, 34, 24, 36, 7, 129, 66, 11, 40, 68, 130, 19, 13, 48, 14, 72, 21, 132, 35, 26, 80, 37, 25, 22, 136, 38, 96, 67, 41, 144, 28, 69, 42, 49, 74, 160, 192, 70, 44, 131, 81, 50, 73, 15, 133, 52, 23, 134, 76, 137, 82, 56, 27, 97, 39, 84, 138, 145, 29, 43, 98, 88, 140, 30, 146, 71, 161, 45, 100, 51, 148, 46, 75, 104, 162, 53, 193, 152, 77, 164, 54, 83, 57, 112, 135, 78, 194, 85, 58, 168, 139, 99, 86, 60, 89, 196, 141, 101, 147, 176, 142, 31, 200, 90, 149, 102, 105, 163, 92, 47, 208, 150, 153, 165, 106, 55, 113, 154, 79, 108, 224, 166, 195, 59, 169, 114, 156, 87, 197, 116, 170, 61, 177, 91, 198, 172, 120, 201, 62, 143, 103, 178, 93, 202, 107, 180, 151, 209, 94, 204, 155, 210, 109, 184, 115, 167, 225, 157, 110, 117, 212, 171, 226, 216, 158, 118, 173, 121, 199, 179, 228, 174, 122, 203, 63, 181, 232, 124, 205, 182, 211, 185, 240, 206, 95, 213, 186, 227, 111, 214, 188, 217, 229, 159, 119, 218, 230, 233, 175, 123, 220, 183, 234, 125, 241, 207, 187, 236, 126, 242, 244, 189, 215, 219, 231, 248, 190, 221, 235, 222, 237, 243, 238, 245, 127, 191, 246, 249, 250, 252, 223, 239, 251, 247, 253, 254, 255 };
static const uint16_t Q_0_Nminus1_9[512] = { 0, 1, 2, 4, 8, 16, 32, 3, 5, 64, 9, 6, 17, 10, 18, 128, 12, 33, 65, 20, 256, 34, 24, 36, 7, 129, 66, 11, 40, 68, 130, 19, 13, 48, 14, 72, 257, 21, 132, 35, 258, 26, 80, 37, 25, 22, 136, 260, 264, 38, 96, 67, 41, 144, 28, 69, 42, 49, 74, 272, 160, 288, 192, 70, 44, 131, 81, 50, 73, 15, 320, 133, 52, 23, 134, 384, 76, 137, 82, 56, 27, 97, 39, 259, 84, 138, 145, 261, 29, 43, 98, 88, 140, 30, 146, 71, 262, 265, 161, 45, 100, 51, 148, 46, 75, 266, 273, 104, 162, 53, 193, 152, 77, 164, 268, 274, 54, 83, 57, 112, 135, 78, 289, 194, 85, 276, 58, 168, 139, 99, 86, 60, 280, 89, 290, 196, 141, 101, 147, 176, 142, 321, 31, 200, 90, 292, 322, 263, 149, 102, 105, 304, 296, 163, 92, 47, 267, 385, 324, 208, 386, 150, 153, 165, 106, 55, 328, 113, 154, 79, 269, 108, 224, 166, 195, 270, 275, 291, 59, 169, 114, 277, 156, 87, 197, 116, 170, 61, 281, 278, 177, 293, 388, 91, 198, 172, 120, 201, 336, 62, 282, 143, 103, 178, 294, 93, 202, 323, 392, 297, 107, 180, 151, 209, 284, 94, 204, 298, 400, 352, 325, 155, 210, 305, 300, 109, 184, 115, 167, 225, 326, 306, 157, 329, 110, 117, 212, 171, 330, 226, 387, 308, 216, 416, 271, 279, 158, 337, 118, 332, 389, 173, 121, 199, 179, 228, 338, 312, 390, 174, 393, 283, 122, 448, 353, 203, 63, 340, 394, 181, 295, 285, 232, 124, 205, 182, 286, 299, 354, 211, 401, 185, 396, 344, 240, 206, 95, 327, 402, 356, 307, 301, 417, 213, 186, 404, 227, 418, 302, 360, 111, 331, 214, 309, 188, 449, 217, 408, 229, 159, 420, 310, 333, 119, 339, 218, 368, 230, 391, 313, 450, 334, 233, 175, 123, 341, 220, 314, 424, 395, 355, 287, 183, 234, 125, 342, 316, 241, 345, 452, 397, 403, 207, 432, 357, 187, 236, 126, 242, 398, 346, 456, 358, 405, 303, 244, 189, 361, 215, 348, 419, 406, 464, 362, 409, 219, 311, 421, 410, 231, 248, 369, 190, 364, 335, 480, 315, 221, 370, 422, 425, 451, 235, 412, 343, 372, 317, 222, 426, 453, 237, 433, 347, 243, 454, 318, 376, 428, 238, 359, 457, 399, 434, 349, 245, 458, 363, 127, 191, 407, 436, 465, 246, 350, 460, 249, 411, 365, 440, 374, 423, 466, 250, 371, 481, 413, 366, 468, 429, 252, 373, 482, 427, 414, 223, 472, 455, 377, 435, 319, 484, 430, 488, 239, 378, 459, 437, 380, 461, 496, 351, 467, 438, 251, 462, 442, 441, 469, 247, 367, 253, 375, 444, 470, 483, 415, 485, 473, 474, 254, 379, 431, 489, 486, 476, 439, 490, 463, 381, 497, 492, 443, 382, 498, 445, 471, 500, 446, 475, 487, 504, 255, 477, 491, 478, 383, 493, 499, 502, 494, 501, 447, 505, 506, 479, 508, 495, 503, 507, 509, 510, 511 };
static const uint16_t Q_0_Nminus1_10[1024] = { 0, 1, 2, 4, 8, 16, 32, 3, 5, 64, 9, 6, 17, 10, 18, 128, 12, 33, 65, 20, 256, 34, 24, 36, 7, 129, 66, 512, 11, 40, 68, 130, 19, 13, 48, 14, 72, 257, 21, 132, 35, 258, 26, 513, 80, 37, 25, 22, 136, 260, 264, 38, 514, 96, 67, 41, 144, 28, 69, 42, 516, 49, 74, 272, 160, 520, 288, 528, 192, 544, 70, 44, 131, 81, 50, 73, 15, 320, 133, 52, 23, 134, 384, 76, 137, 82, 56, 27, 97, 39, 259, 84, 138, 145, 261, 29, 43, 98, 515, 88, 140, 30, 146, 71, 262, 265, 161, 576, 45, 100, 640, 51, 148, 46, 75, 266, 273, 517, 104, 162, 53, 193, 152, 77, 164, 768, 268, 274, 518, 54, 83, 57, 521, 112, 135, 78, 289, 194, 85, 276, 522, 58, 168, 139, 99, 86, 60, 280, 89, 290, 529, 524, 196, 141, 101, 147, 176, 142, 530, 321, 31, 200, 90, 545, 292, 322, 532, 263, 149, 102, 105, 304, 296, 163, 92, 47, 267, 385, 546, 324, 208, 386, 150, 153, 165, 106, 55, 328, 536, 577, 548, 113, 154, 79, 269, 108, 578, 224, 166, 519, 552, 195, 270, 641, 523, 275, 580, 291, 59, 169, 560, 114, 277, 156, 87, 197, 116, 170, 61, 531, 525, 642, 281, 278, 526, 177, 293, 388, 91, 584, 769, 198, 172, 120, 201, 336, 62, 282, 143, 103, 178, 294, 93, 644, 202, 592, 323, 392, 297, 770, 107, 180, 151, 209, 284, 648, 94, 204, 298, 400, 608, 352, 325, 533, 155, 210, 305, 547, 300, 109, 184, 534, 537, 115, 167, 225, 326, 306, 772, 157, 656, 329, 110, 117, 212, 171, 776, 330, 226, 549, 538, 387, 308, 216, 416, 271, 279, 158, 337, 550, 672, 118, 332, 579, 540, 389, 173, 121, 553, 199, 784, 179, 228, 338, 312, 704, 390, 174, 554, 581, 393, 283, 122, 448, 353, 561, 203, 63, 340, 394, 527, 582, 556, 181, 295, 285, 232, 124, 205, 182, 643, 562, 286, 585, 299, 354, 211, 401, 185, 396, 344, 586, 645, 593, 535, 240, 206, 95, 327, 564, 800, 402, 356, 307, 301, 417, 213, 568, 832, 588, 186, 646, 404, 227, 896, 594, 418, 302, 649, 771, 360, 539, 111, 331, 214, 309, 188, 449, 217, 408, 609, 596, 551, 650, 229, 159, 420, 310, 541, 773, 610, 657, 333, 119, 600, 339, 218, 368, 652, 230, 391, 313, 450, 542, 334, 233, 555, 774, 175, 123, 658, 612, 341, 777, 220, 314, 424, 395, 673, 583, 355, 287, 183, 234, 125, 557, 660, 616, 342, 316, 241, 778, 563, 345, 452, 397, 403, 207, 674, 558, 785, 432, 357, 187, 236, 664, 624, 587, 780, 705, 126, 242, 565, 398, 346, 456, 358, 405, 303, 569, 244, 595, 189, 566, 676, 361, 706, 589, 215, 786, 647, 348, 419, 406, 464, 680, 801, 362, 590, 409, 570, 788, 597, 572, 219, 311, 708, 598, 601, 651, 421, 792, 802, 611, 602, 410, 231, 688, 653, 248, 369, 190, 364, 654, 659, 335, 480, 315, 221, 370, 613, 422, 425, 451, 614, 543, 235, 412, 343, 372, 775, 317, 222, 426, 453, 237, 559, 833, 804, 712, 834, 661, 808, 779, 617, 604, 433, 720, 816, 836, 347, 897, 243, 662, 454, 318, 675, 618, 898, 781, 376, 428, 665, 736, 567, 840, 625, 238, 359, 457, 399, 787, 591, 678, 434, 677, 349, 245, 458, 666, 620, 363, 127, 191, 782, 407, 436, 626, 571, 465, 681, 246, 707, 350, 599, 668, 790, 460, 249, 682, 573, 411, 803, 789, 709, 365, 440, 628, 689, 374, 423, 466, 793, 250, 371, 481, 574, 413, 603, 366, 468, 655, 900, 805, 615, 684, 710, 429, 794, 252, 373, 605, 848, 690, 713, 632, 482, 806, 427, 904, 414, 223, 663, 692, 835, 619, 472, 455, 796, 809, 714, 721, 837, 716, 864, 810, 606, 912, 722, 696, 377, 435, 817, 319, 621, 812, 484, 430, 838, 667, 488, 239, 378, 459, 622, 627, 437, 380, 818, 461, 496, 669, 679, 724, 841, 629, 351, 467, 438, 737, 251, 462, 442, 441, 469, 247, 683, 842, 738, 899, 670, 783, 849, 820, 728, 928, 791, 367, 901, 630, 685, 844, 633, 711, 253, 691, 824, 902, 686, 740, 850, 375, 444, 470, 483, 415, 485, 905, 795, 473, 634, 744, 852, 960, 865, 693, 797, 906, 715, 807, 474, 636, 694, 254, 717, 575, 913, 798, 811, 379, 697, 431, 607, 489, 866, 723, 486, 908, 718, 813, 476, 856, 839, 725, 698, 914, 752, 868, 819, 814, 439, 929, 490, 623, 671, 739, 916, 463, 843, 381, 497, 930, 821, 726, 961, 872, 492, 631, 729, 700, 443, 741, 845, 920, 382, 822, 851, 730, 498, 880, 742, 445, 471, 635, 932, 687, 903, 825, 500, 846, 745, 826, 732, 446, 962, 936, 475, 853, 867, 637, 907, 487, 695, 746, 828, 753, 854, 857, 504, 799, 255, 964, 909, 719, 477, 915, 638, 748, 944, 869, 491, 699, 754, 858, 478, 968, 383, 910, 815, 976, 870, 917, 727, 493, 873, 701, 931, 756, 860, 499, 731, 823, 922, 874, 918, 502, 933, 743, 760, 881, 494, 702, 921, 501, 876, 847, 992, 447, 733, 827, 934, 882, 937, 963, 747, 505, 855, 924, 734, 829, 965, 938, 884, 506, 749, 945, 966, 755, 859, 940, 830, 911, 871, 639, 888, 479, 946, 750, 969, 508, 861, 757, 970, 919, 875, 862, 758, 948, 977, 923, 972, 761, 877, 952, 495, 703, 935, 978, 883, 762, 503, 925, 878, 735, 993, 885, 939, 994, 980, 926, 764, 941, 967, 886, 831, 947, 507, 889, 984, 751, 942, 996, 971, 890, 509, 949, 973, 1000, 892, 950, 863, 759, 1008, 510, 979, 953, 763, 974, 954, 879, 981, 982, 927, 995, 765, 956, 887, 985, 997, 986, 943, 891, 998, 766, 511, 988, 1001, 951, 1002, 893, 975, 894, 1009, 955, 1004, 1010, 957, 983, 958, 987, 1012, 999, 1016, 767, 989, 1003, 990, 1005, 959, 1011, 1013, 895, 1006, 1014, 1017, 1018, 991, 1020, 1007, 1015, 1019, 1021, 1022, 1023 };

static const uint16_t *Q_0_Nminus1[11] = { NULL, Q_0_Nminus1_1, Q_0_Nminus1_2, Q_0_Nminus1_3, Q_0_Nminus1_4,
		Q_0_Nminus1_5, Q_0_Nminus1_6, Q_0_Nminus1_7, Q_0_Nminus1_8, Q_0_Nminus1_9, Q_0_Nminus1_10 };

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
                                       uint8_t *L);


#ifndef __SSE3__
#warning SSE3 instruction set not preset
#define _mm_abs_epi16(xmmx) _mm_xor_si128((xmmx),_mm_cmpgt_epi16(_mm_setzero_si128(),(xmmx)))
#define _mm_sign_epi16(xmmx,xmmy) _mm_xor_si128((xmmx),_mm_cmpgt_epi16(_mm_setzero_si128(),(xmmy)))
#define _mm_hadd_epi32(xmmx,xmmy) _mm_unpacklo_epi64(_mm_add_epi32(_mm_shuffle_epi32((xmmx),_MM_SHUFFLE(0,2,0,2)),_mm_shuffle_epi32((xmmx),_MM_SHUFFLE(1,3,1,3))),_mm_add_epi32(_mm_shuffle_epi32((xmmy),_MM_SHUFFLE(0,2,0,2)),_mm_shuffle_epi32((xmmy),_MM_SHUFFLE(1,3,1,3))))

// variant from lte_ul_channel_estimation.c and dlsch_demodulation.c and pmch.c
//#define _mm_abs_epi16(xmmx) _mm_add_epi16(_mm_xor_si128((xmmx),_mm_cmpgt_epi16(_mm_setzero_si128(),(xmmx))),_mm_srli_epi16(_mm_cmpgt_epi16(_mm_setzero_si128(),(xmmx)),15))

// variant from cdot_prod.c
//#define _mm_abs_epi16(xmmx) xmmx=_mm_xor_si128((xmmx),_mm_cmpgt_epi16(_mm_setzero_si128(),(xmmx)))

#define _mm_shuffle_epi8(xmmx,xmmy) ssp_shuffle_epi8_SSE2(xmmx,xmmy)

/*! \brief Helper function.
 *
 * this source code fragment is Copyright (c) 2006-2008 Advanced Micro Devices, Inc.
 * It is licensed under Apache License 2.0 (compatible to our GPL3).
 * see http://sourceforge.net/projects/sseplus
 * \author Advanced Micro Devices, Inc.
 * \date 2006-2008
 * \copyright Apache License 2.0
 */
static inline __m128i ssp_comge_epi8_SSE2(__m128i a, __m128i b) {
  __m128i c;
  c = _mm_cmpgt_epi8( a, b );
  a = _mm_cmpeq_epi8( a, b );
  a = _mm_or_si128  ( a, c );
  return a;
}

/*! \brief SSE2 emulation of SSE3 _mm_shuffle_epi8().
 *
 * this source code fragment is Copyright (c) 2006-2008 Advanced Micro Devices, Inc.
 * It is licensed under Apache License 2.0 (compatible to our GPL3).
 * see http://sourceforge.net/projects/sseplus
 * \author Advanced Micro Devices, Inc.
 * \date 2006-2008
 * \copyright Apache License 2.0
 */
}
#endif // __SSE3__


typedef struct {
  uint16_t rnti;
  uint8_t dci_format;
  // n_CCE index of first CCE for PDCCH reception
  int n_CCE;
  // N_CCE is L, or number of CCEs for DCI
  int N_CCE;
  uint8_t payloadSize;
  uint8_t payloadBits[16];
  //fapi_nr_dci_pdu_rel15_t dci;
} fapi_nr_dci_indication_pdu_t;

typedef struct {
  uint16_t SFN;
  uint8_t slot;
  uint16_t number_of_dcis;
  fapi_nr_dci_indication_pdu_t dci_list[10];
} fapi_nr_dci_indication_t;

typedef uint16_t rnti_t;

typedef struct decoder_node_t_s {
  struct decoder_node_t_s *left;
  struct decoder_node_t_s *right;
  int level;
  int leaf;
  int Nv;
  int first_leaf_index;
  int all_frozen;
  int16_t *alpha;
  int16_t *beta;
} decoder_node_t;

typedef struct decoder_tree_t_s {
  decoder_node_t *root;
  int num_nodes;
} decoder_tree_t;

struct nrPolar_params {
  //messageType: 0=PBCH, 1=DCI, -1=UCI
  int idx; //idx = (messageType * messageLength * aggregation_prime);
  struct nrPolar_params *nextPtr;

  uint8_t n_max;
  uint8_t i_il;
  uint8_t i_seg;
  uint8_t n_pc;
  uint8_t n_pc_wm;
  uint8_t i_bil;
  uint16_t payloadBits;
  uint16_t encoderLength;
  uint8_t crcParityBits;
  uint8_t crcCorrectionBits;
  uint16_t K;
  uint16_t N;
  uint8_t n;
  uint32_t crcBit;

  uint16_t *interleaving_pattern;
  uint16_t *deinterleaving_pattern;
  uint16_t *rate_matching_pattern;
  const uint16_t *Q_0_Nminus1;
  int16_t *Q_I_N;
  int16_t *Q_F_N;
  int16_t *Q_PC_N;
  uint8_t *information_bit_pattern;
  uint16_t *channel_interleaver_pattern;
  //uint32_t crc_polynomial;

  uint8_t **crc_generator_matrix; //G_P
  uint8_t **G_N;
  uint64_t **G_N_tab;
  int groupsize;
  int *rm_tab;
  uint64_t cprime_tab0[32][256];
  uint64_t cprime_tab1[32][256];
  uint64_t B_tab0[32][256];
  uint64_t B_tab1[32][256];
  uint8_t **extended_crc_generator_matrix;
  //lowercase: bits, Uppercase: Bits stored in bytes
  //polar_encoder vectors
  uint8_t *nr_polar_crc;
  uint8_t *nr_polar_aPrime;
  uint8_t *nr_polar_APrime;
  uint8_t *nr_polar_D;
  uint8_t *nr_polar_E;

  //Polar Coding vectors
  uint8_t *nr_polar_A;
  uint8_t *nr_polar_CPrime;
  uint8_t *nr_polar_B;
  uint8_t *nr_polar_U;

  decoder_tree_t tree;
} __attribute__ ((__packed__));
typedef struct nrPolar_params t_nrPolar_params;

#define NR_POLAR_DCI_MESSAGE_TYPE 1


typedef struct {
  /// \brief Pointers to extracted PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_ext;
  /// \brief Pointers to extracted and compensated PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp;
  /// \brief Hold the channel estimates in frequency domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: samples? [0..symbols_per_tti*(ofdm_symbol_size+LTE_CE_FILTER_LENGTH)[
  int32_t **dl_ch_estimates;
  /// \brief Hold the channel estimates in time domain (used for tracking).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: samples? [0..2*ofdm_symbol_size[
  int32_t **dl_ch_estimates_time;
  /// \brief Pointers to extracted channel estimates of PDCCH symbols.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_estimates_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-gNB detection.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-gNB detection.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..]
  int32_t **rho;
  /// \brief Pointer to llrs, 4-bit resolution.
  /// - first index: ? [0..48*N_RB_DL[
  int16_t *llr;
  /// \brief Pointer to llrs, 16-bit resolution.
  /// - first index: ? [0..96*N_RB_DL[
  int16_t *llr16;
  /// \brief \f$\overline{w}\f$ from 36-211.
  /// - first index: ? [0..48*N_RB_DL[
  int16_t *wbar;
  /// \brief PDCCH/DCI e-sequence (input to rate matching).
  /// - first index: ? [0..96*N_RB_DL[
  int16_t *e_rx;
  /// Total number of PDU errors (diagnostic mode)
  uint32_t dci_errors;
  /// Total number of PDU received
  uint32_t dci_received;
  /// Total number of DCI False detection (diagnostic mode)
  uint32_t dci_false;
  /// Total number of DCI missed (diagnostic mode)
  uint32_t dci_missed;
  /// nCCE for PDCCH per subframe
  uint8_t nCCE[10];
  //Check for specific DCIFormat and AgregationLevel
  uint8_t dciFormat;
  uint8_t agregationLevel;
  int nb_search_space;
//   fapi_nr_dl_config_dci_dl_pdu_rel15_t pdcch_config[FAPI_NR_MAX_SS];
  // frame and slot for sib1 in initial sync
  uint16_t sfn;
  uint16_t slot;
  /*
#ifdef NR_PDCCH_DEFS_NR_UE
  int nb_searchSpaces;
  // CORESET structure, where maximum number of CORESETs to be handled is 3 (according to 38.331 V15.1.0)
  NR_UE_PDCCH_CORESET coreset[NR_NBR_CORESET_ACT_BWP];
  // SEARCHSPACE structure, where maximum number of SEARCHSPACEs to be handled is 10 (according to 38.331 V15.1.0)
  // Each SearchSpace is associated with one ControlResourceSet
  NR_UE_PDCCH_SEARCHSPACE searchSpace[NR_NBR_SEARCHSPACE_ACT_BWP];

  int n_RB_BWP[NR_NBR_SEARCHSPACE_ACT_BWP];
  uint32_t nb_search_space;
  #endif*/
} NR_UE_PDCCH;

typedef struct {
  /// frequency_domain_resource;
  uint8_t frequency_domain_resource[6];
  uint8_t StartSymbolIndex;
  uint8_t duration;
  uint8_t CceRegMappingType; //  interleaved or noninterleaved
  uint8_t RegBundleSize;     //  valid if CCE to REG mapping type is interleaved type
  uint8_t InterleaverSize;   //  valid if CCE to REG mapping type is interleaved type
  uint8_t ShiftIndex;        //  valid if CCE to REG mapping type is interleaved type
  uint8_t CoreSetType;
  uint8_t precoder_granularity;
  uint16_t pdcch_dmrs_scrambling_id;
  uint16_t scrambling_rnti;
  uint8_t tci_state_pdcch;
  uint8_t tci_present_in_dci;
} fapi_nr_coreset_t;


typedef struct {
  uint16_t rnti;
  uint16_t BWPSize;
  uint16_t BWPStart;
  uint8_t SubcarrierSpacing;
  fapi_nr_coreset_t coreset;
  uint8_t number_of_candidates;
  uint16_t CCE[64];
  uint8_t L[64];
  // 3GPP TS 38.212 Sec. 7.3.1.0, 3GPP TS 138.131 sec. 6.3.2 (SearchSpace)
  // The maximum number of DCI lengths allowed by the spec are 4, with max 3 for C-RNTI.
  // But a given search space may only support a maximum of 2 DCI formats at a time
  // depending on its search space type configured by RRC. Hence for blind decoding, UE
  // needs to monitor only upto 2 DCI lengths for a given search space.
  uint8_t num_dci_options;  // Num DCIs the UE actually needs to decode (1 or 2)
  uint8_t dci_length_options[2];
  uint8_t dci_format_options[2];
} fapi_nr_dl_config_dci_dl_pdu_rel15_t;


typedef struct {
    t_nrPolar_params *polarList;

} PHY_VARS_NR_UE;

#define AssertFatal(cOND, fORMAT, aRGS...)          _Assert_(cOND, _Assert_Exit_, fORMAT, ##aRGS)
#define _Assert_Exit_                           \
    fprintf(stderr, "\nExiting execution\n");   \
    fflush(stdout);                             \
    fflush(stderr);                             \
    abort();

#define _Assert_(cOND, aCTION, fORMAT, aRGS...)             \
do {                                                        \
    if (!(cOND)) {                                          \
        fprintf(stderr, "\nAssertion ("#cOND") failed!\n"   \
                "In %s() %s:%d\n" fORMAT,                   \
                __FUNCTION__, __FILE__, __LINE__, ##aRGS);  \
        aCTION;                                             \
    }						\
} while(0)


static inline  uint32_t lte_gold_generic(uint32_t *x1, uint32_t *x2, uint8_t reset)
{
  int32_t n;

  // 3GPP 3x.211
  // Nc = 1600
  // c(n)     = [x1(n+Nc) + x2(n+Nc)]mod2
  // x1(n+31) = [x1(n+3)                     + x1(n)]mod2
  // x2(n+31) = [x2(n+3) + x2(n+2) + x2(n+1) + x2(n)]mod2
  if (reset)
  {
      // Init value for x1: x1(0) = 1, x1(n) = 0, n=1,2,...,30
      // x1(31) = [x1(3) + x1(0)]mod2 = 1
      *x1 = 1 + (1U<<31);
      // Init value for x2: cinit = sum_{i=0}^30 x2*2^i
      // x2(31) = [x2(3)    + x2(2)    + x2(1)    + x2(0)]mod2
      //        =  (*x2>>3) ^ (*x2>>2) + (*x2>>1) + *x2
      *x2 = *x2 ^ ((*x2 ^ (*x2>>1) ^ (*x2>>2) ^ (*x2>>3))<<31);

      // x1 and x2 contain bits n = 0,1,...,31

      // Nc = 1600 bits are skipped at the beginning
      // i.e., 1600 / 32 = 50 32bit words

      for (n = 1; n < 50; n++)
      {
          // Compute x1(0),...,x1(27)
          *x1 = (*x1>>1) ^ (*x1>>4);
          // Compute x1(28),..,x1(31) and xor
          *x1 = *x1 ^ (*x1<<31) ^ (*x1<<28);
          // Compute x2(0),...,x2(27)
          *x2 = (*x2>>1) ^ (*x2>>2) ^ (*x2>>3) ^ (*x2>>4);
          // Compute x2(28),..,x2(31) and xor
          *x2 = *x2 ^ (*x2<<31) ^ (*x2<<30) ^ (*x2<<29) ^ (*x2<<28);
      }
  }

  *x1 = (*x1>>1) ^ (*x1>>4);
  *x1 = *x1 ^ (*x1<<31) ^ (*x1<<28);
  *x2 = (*x2>>1) ^ (*x2>>2) ^ (*x2>>3) ^ (*x2>>4);
  *x2 = *x2 ^ (*x2<<31) ^ (*x2<<30) ^ (*x2<<29) ^ (*x2<<28);

  // c(n) = [x1(n+Nc) + x2(n+Nc)]mod2
  return(*x1^*x2);
}
#define exit_fun(msg) exit_function(__FILE__,__FUNCTION__,__LINE__,msg)


uint8_t nr_dci_decoding_procedure(PHY_VARS_NR_UE *ue,
                                  fapi_nr_dci_indication_t *dci_ind,
                                  fapi_nr_dl_config_dci_dl_pdu_rel15_t *rel15,
                                  NR_UE_PDCCH *pdcch_vars);

t_nrPolar_params *nr_polar_params (int8_t messageType,
                                   uint16_t messageLength,
                                   uint8_t aggregation_level,
	 		           int decoding_flag,
				   t_nrPolar_params **polarList_ext);

void init_polar_deinterleaver_table(t_nrPolar_params *polarParams);
void build_polar_tables(t_nrPolar_params *polarParams);
void build_decoder_tree(t_nrPolar_params *polarParams);
decoder_node_t *add_nodes(int level, int first_leaf_index, t_nrPolar_params *polarParams);
decoder_node_t *new_decoder_node(int first_leaf_index, int level);
void nr_polar_channel_interleaver_pattern(uint16_t *cip,
					  uint8_t I_BIL,
					  uint16_t E);
static int intcmp(const void *p1,const void *p2);
void nr_polar_info_bit_pattern(uint8_t *ibp,
			       int16_t *Q_I_N,
			       int16_t *Q_F_N,
			       uint16_t *J,
			       const uint16_t *Q_0_Nminus1,
			       uint16_t K,
			       uint16_t N,
			       uint16_t E,
			       uint8_t n_PC);
static const uint8_t nr_polar_subblock_interleaver_pattern[32] = {0,1,2,4,3,5,6,7,8,16,9,17,10,18,11,19,12,20,13,21,14,22,15,23,24,25,26,28,27,29,30,31};

void nr_polar_rate_matching_pattern(uint16_t *rmp,
                                    uint16_t *J,
                                    const uint8_t *P_i_,
                                    uint16_t K,
                                    uint16_t N,
                                    uint16_t E);
void nr_polar_interleaving_pattern(uint16_t K, uint8_t I_IL, uint16_t *PI_k_);
const uint16_t* nr_polar_sequence_pattern(uint8_t n);
uint8_t** nr_polar_kronecker_power_matrices(uint8_t n);
uint32_t nr_polar_output_length(uint16_t K,
				uint16_t E,
				uint8_t n_max);
uint8_t **crc24c_generator_matrix(uint16_t payloadSizeBits);
static void nr_polar_init(t_nrPolar_params * *polarParams,
                          int8_t messageType,
                          uint16_t messageLength,
                          uint8_t aggregation_level,
			  int decoder_flag);
uint16_t nr_polar_aggregation_prime (uint8_t aggregation_level);
void nr_pdcch_unscrambling(int16_t *z,
                           uint16_t scrambling_RNTI,
                           uint32_t length,
                           uint16_t pdcch_DMRS_scrambling_id,
                           int16_t *z2);
uint32_t polar_decoder_int16(int16_t *input,
                             uint64_t *out,
                             uint8_t ones_flag,
                             const t_nrPolar_params *polarParams);
void nr_polar_rate_matching_int16(int16_t *input,
				  int16_t *output,
				  uint16_t *rmp,
				  uint16_t K,
				  uint16_t N,
				  uint16_t E);
void generic_polar_decoder(const t_nrPolar_params *pp,decoder_node_t *node);
void computeBeta(const t_nrPolar_params *pp,decoder_node_t *node);
void applyGtoright(const t_nrPolar_params *pp,decoder_node_t *node);
void generic_polar_decoder(const t_nrPolar_params *pp,decoder_node_t *node);
unsigned int crcbit (unsigned char * inputptr, int octetlen, unsigned int poly);
void applyFtoleft(const t_nrPolar_params *pp, decoder_node_t *node);