#include <stdio.h>
#include "polyvec.h"
#include "reduce.h"
#include "fips202x4.h"
#include "cbd.h"


void polyvec_compress9(unsigned char *r, const polyvec *a)
{
	int i, j, k;
	uint16_t t[8];
	uint16_t cpbytes = ((PARAM_N * 9) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 8; j++)
		{
			for (k = 0; k<8; k++)
				t[k] = ((((uint32_t)a->vec[i].coeffs[8 * j + k] << 9) + PARAM_Q / 2) / PARAM_Q) & 0x1ff;

			r[9 * j + 0] = t[0] & 0xff;
			r[9 * j + 1] = (t[0] >> 8) | ((t[1] & 0x7f) << 1);
			r[9 * j + 2] = (t[1] >> 7) | ((t[2] & 0x3f) << 2);
			r[9 * j + 3] = (t[2] >> 6) | ((t[3] & 0x1f) << 3);
			r[9 * j + 4] = (t[3] >> 5) | ((t[4] & 0x0f) << 4);
			r[9 * j + 5] = (t[4] >> 4) | ((t[5] & 0x07) << 5);
			r[9 * j + 6] = (t[5] >> 3) | ((t[6] & 0x03) << 6);
			r[9 * j + 7] = (t[6] >> 2) | ((t[7] & 0x01) << 7);
			r[9 * j + 8] = (t[7] >> 1);
		}
		r += cpbytes;
	}
}
void polyvec_decompress9(polyvec *r, const unsigned char *a)
{
	int i, j;
	uint16_t cpbytes = ((PARAM_N * 9) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 8; j++)
		{
			r->vec[i].coeffs[8 * j + 0] = (((a[9 * j + 0] | (((uint32_t)a[9 * j + 1] & 0x01) << 8)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 1] = ((((a[9 * j + 1] >> 1) | (((uint32_t)a[9 * j + 2] & 0x03) << 7)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 2] = ((((a[9 * j + 2] >> 2) | (((uint32_t)a[9 * j + 3] & 0x07) << 6)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 3] = ((((a[9 * j + 3] >> 3) | (((uint32_t)a[9 * j + 4] & 0x0f) << 5)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 4] = ((((a[9 * j + 4] >> 4) | (((uint32_t)a[9 * j + 5] & 0x1f) << 4)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 5] = ((((a[9 * j + 5] >> 5) | (((uint32_t)a[9 * j + 6] & 0x3f) << 3)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 6] = ((((a[9 * j + 6] >> 6) | (((uint32_t)a[9 * j + 7] & 0x7f) << 2)) * PARAM_Q) + 256) >> 9;
			r->vec[i].coeffs[8 * j + 7] = ((((a[9 * j + 7] >> 7) | (((uint32_t)a[9 * j + 8]) << 1)) * PARAM_Q) + 256) >> 9;
		}
		a += cpbytes;
	}
}

void polyvec_compress10(unsigned char *r, const polyvec *a)
{
	int i, j, k;
	uint16_t t[4];
	uint16_t cpbytes = ((PARAM_N * 10) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 4; j++)
		{
			for (k = 0; k<4; k++)
				t[k] = ((((uint32_t)a->vec[i].coeffs[4 * j + k] << 10) + PARAM_Q / 2) / PARAM_Q) & 0x3ff;

			r[5 * j + 0] = t[0] & 0xff;
			r[5 * j + 1] = (t[0] >> 8) | ((t[1] & 0x3f) << 2);
			r[5 * j + 2] = (t[1] >> 6) | ((t[2] & 0x0f) << 4);
			r[5 * j + 3] = (t[2] >> 4) | ((t[3] & 0x03) << 6);
			r[5 * j + 4] = (t[3] >> 2);
		}
		r += cpbytes;
	}
}
void polyvec_decompress10(polyvec *r, const unsigned char *a)
{
	int i, j;
	uint16_t cpbytes = ((PARAM_N * 10) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 4; j++)
		{
			r->vec[i].coeffs[4 * j + 0] = (((a[5 * j + 0] | (((uint32_t)a[5 * j + 1] & 0x03) << 8)) * PARAM_Q) + 512) >> 10;
			r->vec[i].coeffs[4 * j + 1] = ((((a[5 * j + 1] >> 2) | (((uint32_t)a[5 * j + 2] & 0x0f) << 6)) * PARAM_Q) + 512) >> 10;
			r->vec[i].coeffs[4 * j + 2] = ((((a[5 * j + 2] >> 4) | (((uint32_t)a[5 * j + 3] & 0x3f) << 4)) * PARAM_Q) + 512) >> 10;
			r->vec[i].coeffs[4 * j + 3] = ((((a[5 * j + 3] >> 6) | (((uint32_t)a[5 * j + 4]) << 2)) * PARAM_Q) + 512) >> 10;
		}
		a += cpbytes;
	}
}

void polyvec_compress11(unsigned char *r, const polyvec *a)
{
	int i, j, k;
	uint16_t t[8];
	uint16_t cpbytes = ((PARAM_N * 11) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 8; j++)
		{
			for (k = 0; k<8; k++)
				t[k] = ((((uint32_t)a->vec[i].coeffs[8 * j + k] << 11) + PARAM_Q / 2) / PARAM_Q) & 0x7ff;

			r[11 * j + 0] = t[0] & 0xff;
			r[11 * j + 1] = (t[0] >> 8) | ((t[1] & 0x1f) << 3);
			r[11 * j + 2] = (t[1] >> 5) | ((t[2] & 0x03) << 6);
			r[11 * j + 3] = (t[2] >> 2) & 0xff;
			r[11 * j + 4] = (t[2] >> 10) | ((t[3] & 0x7f) << 1);
			r[11 * j + 5] = (t[3] >> 7) | ((t[4] & 0x0f) << 4);
			r[11 * j + 6] = (t[4] >> 4) | ((t[5] & 0x01) << 7);
			r[11 * j + 7] = (t[5] >> 1) & 0xff;
			r[11 * j + 8] = (t[5] >> 9) | ((t[6] & 0x3f) << 2);
			r[11 * j + 9] = (t[6] >> 6) | ((t[7] & 0x07) << 5);
			r[11 * j + 10] = (t[7] >> 3);
		}
		r += cpbytes;
	}
}
void polyvec_decompress11(polyvec *r, const unsigned char *a)
{
	int i, j;
	uint16_t cpbytes = ((PARAM_N * 11) >> 3);//the bytes for storing a polynomial in compressed form
	for (i = 0; i<PARAM_K; i++)
	{
		for (j = 0; j<PARAM_N / 8; j++)
		{
			r->vec[i].coeffs[8 * j + 0] = (((a[11 * j + 0] | (((uint32_t)a[11 * j + 1] & 0x07) << 8)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 1] = ((((a[11 * j + 1] >> 3) | (((uint32_t)a[11 * j + 2] & 0x3f) << 5)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 2] = ((((a[11 * j + 2] >> 6) | (((uint32_t)a[11 * j + 3] & 0xff) << 2) | (((uint32_t)a[11 * j + 4] & 0x01) << 10)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 3] = ((((a[11 * j + 4] >> 1) | (((uint32_t)a[11 * j + 5] & 0x0f) << 7)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 4] = ((((a[11 * j + 5] >> 4) | (((uint32_t)a[11 * j + 6] & 0x7f) << 4)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 5] = ((((a[11 * j + 6] >> 7) | (((uint32_t)a[11 * j + 7] & 0xff) << 1) | (((uint32_t)a[11 * j + 8] & 0x03) << 9)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 6] = ((((a[11 * j + 8] >> 2) | (((uint32_t)a[11 * j + 9] & 0x1f) << 6)) * PARAM_Q) + 1024) >> 11;
			r->vec[i].coeffs[8 * j + 7] = ((((a[11 * j + 9] >> 5) | (((uint32_t)a[11 * j + 10] & 0xff) << 3)) * PARAM_Q) + 1024) >> 11;
		}
		a += cpbytes;
	}
}

void polyvec_compress(unsigned char *r, const polyvec *a,uint16_t cbits)
{
	switch(cbits)
	{
	case 9:
		polyvec_compress9(r,a);
		break;
	case 10:
		polyvec_compress10(r,a);
		break;
	case 11:
		polyvec_compress11(r,a);
		break;
	default:
		printf("polyvec_compress only supports cbits in {9,10,11}\n");
	}
}

void polyvec_decompress(polyvec *r, const unsigned char *a,uint16_t cbits)
{
  switch(cbits)
	{
	case 9:
		polyvec_decompress9(r,a);
		break;
	case 10:
		polyvec_decompress10(r,a);
		break;
	case 11:
		polyvec_decompress11(r,a);
		break;
	default:
		printf("polyvec_decompress only supports cbits in {9,10,11}\n");
	}
}


/*************************************************
* Name:        polyvec_tobytes
* 
* Description: Serialize vector of polynomials
*
* Arguments:   - unsigned char *r: pointer to output byte array 
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_tobytes(unsigned char *r, const polyvec *a)
{
  int i;
  for(i=0;i<PARAM_K;i++)
    poly_tobytes(r+i*POLY_BYTES, &a->vec[i]);
}

/*************************************************
* Name:        polyvec_frombytes
* 
* Description: De-serialize vector of polynomials;
*              inverse of polyvec_tobytes 
*
* Arguments:   - unsigned char *r: pointer to output byte array 
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_frombytes(polyvec *r, const unsigned char *a)
{
  int i;
  for(i=0;i<PARAM_K;i++)
    poly_frombytes(&r->vec[i], a+i*POLY_BYTES);
}

void polyvec_ntt(polyvec *r)
{
  int i;
  for(i=0;i<PARAM_K;i++)
    poly_ntt(&r->vec[i]);
}

void polyvec_invntt(polyvec *r)
{
  int i;
  for(i=0;i<PARAM_K;i++)
    poly_invntt(&r->vec[i]);
}
 
/*************************************************
* Name:        polyvec_pointwise_acc
* 
* Description: Pointwise multiply elements of a and b and accumulate into r
*
* Arguments: - poly *r:          pointer to output polynomial
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/ 
void polyvec_pointwise_acc(poly *r, const polyvec *a, const polyvec *b)
{
    
    int i,barratshift;
    __m256i t[PARAM_K],dh[PARAM_K],dl[PARAM_K];
    __m256i *pa[PARAM_K],*pb[PARAM_K],*pc;
    __m256i q16x = _mm256_set1_epi16(PARAM_Q);
#if PARAM_Q == 7681
    __m256i qinv16x = _mm256_set1_epi16(57857U); //inverse_mod(q,2^16)
    __m256i montsq16x = _mm256_set1_epi16(5569); // 5569 = 2^{2*16} % q
    barratshift = 13;
#elif PARAM_Q ==12289
    __m256i qinv16x = _mm256_set1_epi16(53249U); //inverse_mod(q,2^16)
    __m256i montsq16x = _mm256_set1_epi16(10952); // 10952 = 2^{2*16} % q
    barratshift = 14;
#else
#error "polyvec_pointwise_acc only supports PARAM_Q in {3329,7681,12289}"
#endif
    
    
    pc = (__m256i*)r->coeffs;
    for(i=0;i<PARAM_K;i++)
    {
        pa[i] = (__m256i*)a->vec[i].coeffs;
        pb[i] = (__m256i*)b->vec[i].coeffs;
    }
    for(i=0;i<PARAM_N/16;i++)
    {
        
        //mul montsq
        dh[0] = _mm256_mulhi_epi16(montsq16x,pa[0][i]);
        dl[0] = _mm256_mullo_epi16(montsq16x,pa[0][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        dh[0] = _mm256_add_epi16(dh[0],q16x);
        
        //mul b
        dl[0] = _mm256_mullo_epi16(dh[0],pb[0][i]);
        dh[0] = _mm256_mulhi_epi16(dh[0],pb[0][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        t[0] = _mm256_add_epi16(dh[0],q16x);
        
        
        //mul montsq
        dh[0] = _mm256_mulhi_epi16(montsq16x,pa[1][i]);
        dl[0] = _mm256_mullo_epi16(montsq16x,pa[1][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        dh[0] = _mm256_add_epi16(dh[0],q16x);
        
        //mul b
        dl[0] = _mm256_mullo_epi16(dh[0],pb[1][i]);
        dh[0] = _mm256_mulhi_epi16(dh[0],pb[1][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        t[1] = _mm256_add_epi16(dh[0],q16x);
        

#if PARAM_K == 2
        t[0] = _mm256_adds_epu16(t[0],t[1]);
#elif PARAM_K ==3 && PARAM_Q == 7681
        //mul montsq
        dh[0] = _mm256_mulhi_epi16(montsq16x,pa[2][i]);
        dl[0] = _mm256_mullo_epi16(montsq16x,pa[2][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        dh[0] = _mm256_add_epi16(dh[0],q16x);
        
        //mul b
        dl[0] = _mm256_mullo_epi16(dh[0],pb[2][i]);
        dh[0] = _mm256_mulhi_epi16(dh[0],pb[2][i]);
        
        //reduce
        dl[0] = _mm256_mullo_epi16(qinv16x,dl[0]);
        dl[0] = _mm256_mulhi_epi16(q16x,dl[0]);
        dh[0] = _mm256_sub_epi16(dh[0],dl[0]);
        t[2] = _mm256_add_epi16(dh[0],q16x);
        
        t[0] = _mm256_add_epi16(t[0],t[1]);
        t[0] = _mm256_add_epi16(t[0],t[2]); 
#endif
        t[1] = _mm256_srli_epi16(t[0],barratshift);     
        t[1] = _mm256_mullo_epi16(t[1],q16x);
        pc[i] = _mm256_subs_epu16(t[0],t[1]);
    }
}

/*************************************************
* Name:        polyvec_add
* 
* Description: Add vectors of polynomials
*
* Arguments: - polyvec *r:       pointer to output vector of polynomials
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/ 
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
  int i;
  for(i=0;i<PARAM_K;i++)
    poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);

}
void polyvec_getnoise(polyvec *r,const unsigned char *seed, unsigned char nonce, uint32_t eta)
{
#if PARAM_K > 4
#error "polyvec_getnoise only supports PARAM_K <= 4"
#endif
	__declspec(align(32)) unsigned char buf[4][ETA_E*PARAM_N / 4];
  unsigned char extseed[4][SEED_BYTES+1];
  int i;

  for(i=0;i<SEED_BYTES;i++)
  {
    extseed[0][i] = seed[i];
    extseed[1][i] = seed[i];
    extseed[2][i] = seed[i];
    extseed[3][i] = seed[i];
  }
    
  extseed[0][SEED_BYTES] = nonce;
  extseed[1][SEED_BYTES] = nonce+1; 
  extseed[2][SEED_BYTES] = nonce+2; 
  extseed[3][SEED_BYTES] = nonce+3; 
    
  shake256x4(buf[0], buf[1], buf[2], buf[3], eta*PARAM_N/4,extseed[0],extseed[1],extseed[2],extseed[3],SEED_BYTES+1);

  for(i=0;i<PARAM_K;i++)
    cbdx(&r->vec[i],buf[i],eta);
}


void polyvec_getnoise_etas(polyvec *r, const unsigned char *seed, unsigned char nonce)
{
#if PARAM_K > 4
#error "polyvec_getnoise only supports PARAM_K <= 4"
#endif
	unsigned char __declspec(align(32)) buf[4][ETA_E*PARAM_N / 4];
	unsigned char extseed[4][SEED_BYTES + 1];
	int i;

	for (i = 0; i < SEED_BYTES; i++)
	{
		extseed[0][i] = seed[i];
		extseed[1][i] = seed[i];
		extseed[2][i] = seed[i];
		extseed[3][i] = seed[i];
	}

	extseed[0][SEED_BYTES] = nonce;
	extseed[1][SEED_BYTES] = nonce + 1;
	extseed[2][SEED_BYTES] = nonce + 2;
	extseed[3][SEED_BYTES] = nonce + 3;

	shake256x4(buf[0], buf[1], buf[2], buf[3], ETA_S*PARAM_N / 4, extseed[0], extseed[1], extseed[2], extseed[3], SEED_BYTES + 1);

#if PARAM_K >= 1
	cbd_etas(&r->vec[0], buf[0]);
#endif

#if PARAM_K >= 2
	cbd_etas(&r->vec[1], buf[1]);
#endif

#if PARAM_K >= 3
	cbd_etas(&r->vec[2], buf[2]);
#endif

#if PARAM_K >= 4
	cbd_etas(&r->vec[3], buf[3]);
#endif
}

void polyvec_getnoise_etae(polyvec *r, const unsigned char *seed, unsigned char nonce)
{
#if PARAM_K > 4
#error "polyvec_getnoise only supports PARAM_K <= 4"
#endif
	unsigned char __declspec(align(32)) buf[4][ETA_E*PARAM_N / 4];
	unsigned char extseed[4][SEED_BYTES + 1];
	int i;

	for (i = 0; i < SEED_BYTES; i++)
	{
		extseed[0][i] = seed[i];
		extseed[1][i] = seed[i];
		extseed[2][i] = seed[i];
		extseed[3][i] = seed[i];
	}

	extseed[0][SEED_BYTES] = nonce;
	extseed[1][SEED_BYTES] = nonce + 1;
	extseed[2][SEED_BYTES] = nonce + 2;
	extseed[3][SEED_BYTES] = nonce + 3;

	shake256x4(buf[0], buf[1], buf[2], buf[3], ETA_E*PARAM_N / 4, extseed[0], extseed[1], extseed[2], extseed[3], SEED_BYTES + 1);

#if PARAM_K >= 1
	cbd_etae(&r->vec[0], buf[0]);
#endif

#if PARAM_K >= 2
	cbd_etae(&r->vec[1], buf[1]);
#endif

#if PARAM_K >= 3
	cbd_etae(&r->vec[2], buf[2]);
#endif

#if PARAM_K >= 4
	cbd_etae(&r->vec[3], buf[3]);
#endif
}
