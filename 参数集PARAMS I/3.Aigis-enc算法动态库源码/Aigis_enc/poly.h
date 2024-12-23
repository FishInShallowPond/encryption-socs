#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

/* 
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*xoeffs[2] + ... + X^{n-1}*coeffs[n-1] 
 */
typedef __declspec(align(32)) struct{
	uint16_t coeffs[PARAM_N];
} poly;

void poly_compress(unsigned char *r, const poly *a); //each coefficient of a is compressed into cbits  
void poly_decompress(poly *r, const unsigned char *a);

void poly_tobytes(unsigned char *r, const poly *a);
void poly_frombytes(poly *r, const unsigned char *a);

void poly_frommsg(poly *r, const unsigned char msg[SEED_BYTES]);
void poly_tomsg(unsigned char msg[SEED_BYTES], const poly *r);

void poly_getnoise(poly *r,const unsigned char *seed, unsigned char nonce,uint32_t eta);
void poly_getnoise4x(poly *r0,poly *r1,poly *r2,poly *r3,const unsigned char *seed, unsigned char nonce, uint32_t eta);
void poly_getnoise3x(poly *r0,poly *r1,poly *r2,const unsigned char *seed, unsigned char nonce, uint32_t eta);
void poly_getnoise_etae4x(poly *r0, poly *r1, poly *r2, poly *r3, const unsigned char *seed, unsigned char nonce);
void poly_getnoise_etae3x(poly *r0, poly *r1, poly *r2, const unsigned char *seed, unsigned char nonce);

void poly_add(poly *r, const poly *a, const poly *b);
void poly_add3(poly *r, const poly *a, const poly *b, const poly *c);
void poly_sub(poly *r, const poly *a, const poly *b);


void poly_ntt(poly *r);
void poly_invntt(poly *r);
#endif
