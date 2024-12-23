#ifndef PARAMS_H
#define PARAMS_H

#define PARAMS 1 //change this to switch the parameter set

#define USE_AES
//#define USE_NTT_SK //store  sk in NTT form

#if (PARAMS == 1) 
#define PARAM_N 256
#define NLOG 8
#define PARAM_Q 7681
#define PARAM_K 2
#define ETA_S 2
#define ETA_E 12
#define POLY_BYTES 416
#define BITS_PK 10
#define BITS_SK 3
#define BITS_C1 9
#define BITS_C2 3
#define SEED_BYTES 32

#elif (PARAMS == 2) 

#define PARAM_N 256
#define NLOG 8
#define PARAM_Q 7681
#define PARAM_K 3
#define ETA_S 1
#define ETA_E 4
#define POLY_BYTES 416
#define BITS_SK 2
#define BITS_PK 9
#define BITS_C1 9
#define BITS_C2 4
#define SEED_BYTES 32

#elif (PARAMS == 3)  

#define PARAM_N 512
#define NLOG 9
#define PARAM_Q 12289
#define PARAM_K 2
#define ETA_S 2
#define ETA_E 8
#define POLY_BYTES 896
#define BITS_SK 3
#define BITS_PK 11
#define BITS_C1 10
#define BITS_C2 4
#define SEED_BYTES 64

#else
#error "PARAMS must be in {1,2,3}"
#endif


#define POLYVEC_BYTES (PARAM_K * POLY_BYTES) 
#define SHORT_SK_BYTES (PARAM_K * BITS_SK * PARAM_N/8)
#define POLY_COMPRESSED_BYTES (BITS_C2 *PARAM_N/8)
#define PK_POLYVEC_COMPRESSED_BYTES  (BITS_PK *PARAM_K *PARAM_N/8)
#define CT_POLYVEC_COMPRESSED_BYTES  (BITS_C1 *PARAM_K *PARAM_N/8)
#define PK_BYTES (SEED_BYTES + PK_POLYVEC_COMPRESSED_BYTES)
#ifndef USE_NTT_SK
#define SK_BYTES (SHORT_SK_BYTES + PK_BYTES + SEED_BYTES + SEED_BYTES)
#else
#define SK_BYTES (POLYVEC_BYTES + PK_BYTES + SEED_BYTES + SEED_BYTES) //use mulit-target resisitant and implicit rejection
#endif
#define CT_BYTES (CT_POLYVEC_COMPRESSED_BYTES + POLY_COMPRESSED_BYTES)
#endif
