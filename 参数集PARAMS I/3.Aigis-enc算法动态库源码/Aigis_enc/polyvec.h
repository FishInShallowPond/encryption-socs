#ifndef POLYVEC_H
#define POLYVEC_H

#include "params.h"
#include "poly.h"

typedef struct{
  poly vec[PARAM_K];
} polyvec;

void polyvec_compress(unsigned char *r, const polyvec *a, uint16_t cbits);//each coefficient of the polynomial is compressed into cbits
void polyvec_decompress(polyvec *r, const unsigned char *a, uint16_t cbits);

void polyvec_tobytes(unsigned char *r, const polyvec *a);
void polyvec_frombytes(polyvec *r, const unsigned char *a);

void polyvec_ntt(polyvec *r);
void polyvec_invntt(polyvec *r);

void polyvec_pointwise_acc(poly *r, const polyvec *a, const polyvec *b);

void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b);
void polyvec_getnoise(polyvec *r,const unsigned char *seed, unsigned char nonce, uint32_t eta);
void polyvec_getnoise_etas(polyvec *r, const unsigned char *seed, unsigned char nonce);
void polyvec_getnoise_etae(polyvec *r, const unsigned char *seed, unsigned char nonce);
#endif
