#ifndef NTT_H
#define NTT_H

#include <stdint.h>

extern const uint16_t zetas_avx[];
extern const uint16_t zetas_inv_avx[];

void ntt(uint16_t* poly);
void invntt(uint16_t* poly);
#endif
