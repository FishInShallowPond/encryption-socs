#include <stdio.h>
#include <immintrin.h>
#include "poly.h"
#include "ntt.h"
#include "reduce.h"
#include "cbd.h"
#include "fips202.h"
#include "fips202x4.h"


void poly_getnoise(poly *r,const unsigned char *seed, unsigned char nonce, uint32_t eta)
{
  __declspec(align(32)) unsigned char buf[8 * PARAM_N];
  unsigned char extseed[SEED_BYTES+1];
  int i;

  for(i=0;i<SEED_BYTES;i++)
    extseed[i] = seed[i];
  extseed[SEED_BYTES] = nonce;
     
  shake256(buf,eta*PARAM_N/4,extseed,SEED_BYTES+1);

  cbdx(r,buf,eta);
}

void poly_getnoise4x(poly *r0,poly *r1,poly *r2,poly *r3,const unsigned char *seed, unsigned char nonce, uint32_t eta)
{
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
  
 cbdx(r0,buf[0],eta);
 cbdx(r1,buf[1],eta);
 cbdx(r2,buf[2],eta);
 cbdx(r3,buf[3],eta);
}

void poly_getnoise3x(poly *r0,poly *r1,poly *r2,const unsigned char *seed, unsigned char nonce, uint32_t eta)
{
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
  

 cbdx(r0,buf[0],eta);
 cbdx(r1,buf[1],eta);
 cbdx(r2,buf[2],eta);
}


void poly_getnoise_etae4x(poly *r0, poly *r1, poly *r2, poly *r3, const unsigned char *seed, unsigned char nonce)
{
	__declspec(align(32)) unsigned char buf[4][ETA_E*PARAM_N / 4];
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

	cbd_etae(r0, buf[0]);
	cbd_etae(r1, buf[1]);
	cbd_etae(r2, buf[2]);
	cbd_etae(r3, buf[3]);
}

void poly_getnoise_etae3x(poly *r0, poly *r1, poly *r2, const unsigned char *seed, unsigned char nonce)
{
	__declspec(align(32)) unsigned char buf[4][ETA_E*PARAM_N / 4];
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

	cbd_etae(r0, buf[0]);
	cbd_etae(r1, buf[1]);
	cbd_etae(r2, buf[2]);
}
void poly_compress(unsigned char *r, const poly *a)
{
#if BITS_C2 == 3
	unsigned int i, j, k = 0;
	uint32_t t[8];
	for(i=0;i<PARAM_N;i+=8)
	{
		for(j=0;j<8;j++)
			t[j] = ((((uint32_t)a->coeffs[i+j] << 3) + PARAM_Q/2)/PARAM_Q) & 7;

		r[k]   =  t[0]       | (t[1] << 3) | (t[2] << 6);
		r[k+1] = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7);
		r[k+2] = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
		k += 3;
	}
#elif BITS_C2 == 4
	unsigned int i;
	uint32_t t[2];
	for (i = 0; i<PARAM_N / 2; i++)
	{
		t[0] = ((((uint32_t)a->coeffs[2 * i] << 4) + PARAM_Q / 2) / PARAM_Q) & 0xf;
		t[1] = ((((uint32_t)a->coeffs[2 * i + 1] << 4) + PARAM_Q / 2) / PARAM_Q) & 0xf;
		r[i] = t[0] | (t[1] << 4);
	}
#else
#error "poly_compress only supports BITS_C2 in {3,4}"
#endif
}

void poly_decompress(poly *r, const unsigned char *a)
{
	unsigned int i;
#if BITS_C2 == 3
	for (i = 0; i<PARAM_N; i += 8)
	{
		r->coeffs[i + 0] = (((a[0] & 7) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 1] = ((((a[0] >> 3) & 7) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 2] = ((((a[0] >> 6) | ((a[1] << 2) & 4)) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 3] = ((((a[1] >> 1) & 7) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 4] = ((((a[1] >> 4) & 7) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 5] = ((((a[1] >> 7) | ((a[2] << 1) & 6)) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 6] = ((((a[2] >> 2) & 7) * PARAM_Q) + 4) >> 3;
		r->coeffs[i + 7] = ((((a[2] >> 5)) * PARAM_Q) + 4) >> 3;
		a += 3;
	}
#elif BITS_C2 == 4
	__m256i d0, d1;
	__m128i tmp;
	const __m256i mask0f = _mm256_set1_epi32(0xf);
	const __m256i q8x = _mm256_set1_epi32(PARAM_Q);
	const __m256i e8x = _mm256_set1_epi32(8);
	for (i = 0; i<PARAM_N / 32; i++)
	{
		tmp = _mm_loadu_si128((__m128i *)&a[i * 16]);
		d0 = _mm256_cvtepu8_epi32(tmp);
		d1 = _mm256_srli_epi32(d0, 4);
		d0 = _mm256_and_si256(d0, mask0f);

		d0 = _mm256_mullo_epi32(d0, q8x);
		d1 = _mm256_mullo_epi32(d1, q8x);

		d0 = _mm256_add_epi32(d0, e8x);
		d1 = _mm256_add_epi32(d1, e8x);

		d0 = _mm256_srli_epi32(d0, 4);
		d1 = _mm256_slli_epi32(d1, 12);
		d0 = _mm256_blend_epi16(d0, d1, 0xAA);
		_mm256_store_si256((__m256i *)&r->coeffs[32 * i], d0);

		tmp = _mm_srli_si128(tmp, 8);
		d0 = _mm256_cvtepu8_epi32(tmp);
		d1 = _mm256_srli_epi32(d0, 4);
		d0 = _mm256_and_si256(d0, mask0f);

		d0 = _mm256_mullo_epi32(d0, q8x);
		d1 = _mm256_mullo_epi32(d1, q8x);

		d0 = _mm256_add_epi32(d0, e8x);
		d1 = _mm256_add_epi32(d1, e8x);

		d0 = _mm256_srli_epi32(d0, 4);
		d1 = _mm256_slli_epi32(d1, 12);
		d0 = _mm256_blend_epi16(d0, d1, 0xAA);
		_mm256_store_si256((__m256i *)&r->coeffs[32 * i + 16], d0);
	}
#else
#error "poly_decompress only supports BITS_C2 in {3,4}"
#endif
}
void poly_tobytes(unsigned char *r, const poly *a)
{
  
#if PARAM_Q == 7681
  int i,j;
  uint16_t t[8];
  for(i=0;i<PARAM_N/8;i++)
  {
    for(j=0;j<8;j++)
      t[j] = freeze(a->coeffs[8*i+j]);

    r[13*i+ 0] =  t[0]        & 0xff;
    r[13*i+ 1] = (t[0] >>  8) | ((t[1] & 0x07) << 5);
    r[13*i+ 2] = (t[1] >>  3) & 0xff;
    r[13*i+ 3] = (t[1] >> 11) | ((t[2] & 0x3f) << 2);
    r[13*i+ 4] = (t[2] >>  6) | ((t[3] & 0x01) << 7);
    r[13*i+ 5] = (t[3] >>  1) & 0xff;
    r[13*i+ 6] = (t[3] >>  9) | ((t[4] & 0x0f) << 4);
    r[13*i+ 7] = (t[4] >>  4) & 0xff;
    r[13*i+ 8] = (t[4] >> 12) | ((t[5] & 0x7f) << 1);
    r[13*i+ 9] = (t[5] >>  7) | ((t[6] & 0x03) << 6);
    r[13*i+10] = (t[6] >>  2) & 0xff;
    r[13*i+11] = (t[6] >> 10) | ((t[7] & 0x1f) << 3);
    r[13*i+12] = (t[7] >>  5);
  }
#elif PARAM_Q == 12289
  int i,j;
  uint16_t t[4];
  for(i=0;i<PARAM_N/4;i++)
  {
	  for(j=0;j<4;j++)
		  t[j] = freeze(a->coeffs[4*i+j]);

	  r[7*i+ 0] =  t[0]        & 0xff;
	  r[7*i+ 1] = (t[0] >>  8) | ((t[1] & 0x03) << 6);
	  r[7*i+ 2] = (t[1] >>  2) & 0xff;
	  r[7*i+ 3] = (t[1] >> 10) | ((t[2] & 0x0f) << 4);
	  r[7*i+ 4] = (t[2] >>  4) & 0xff;
	  r[7*i+ 5] = (t[2] >>  12) | ((t[3]& 0x3f) <<2);
	  r[7*i+ 6] = (t[3] >>  6);
  }
#endif
}
void poly_frombytes(poly *r, const unsigned char *a)
{
  int i;

#if PARAM_Q == 3329
  for(i=0;i<PARAM_N/2;i++)
  {
	  r->coeffs[2*i+ 0] =  a[3*i+ 0]       | (((uint16_t)a[3*i+ 1] & 0x0f) << 8);
	  r->coeffs[2*i+ 1] = (a[3*i+ 1] >> 4) | (((uint16_t)a[3*i+ 2]      ) << 4);
  }
#elif PARAM_Q == 7681
  for(i=0;i<PARAM_N/8;i++)
  {
    r->coeffs[8*i+0] =  a[13*i+ 0]       | (((uint16_t)a[13*i+ 1] & 0x1f) << 8);
    r->coeffs[8*i+1] = (a[13*i+ 1] >> 5) | (((uint16_t)a[13*i+ 2]       ) << 3) | (((uint16_t)a[13*i+ 3] & 0x03) << 11);
    r->coeffs[8*i+2] = (a[13*i+ 3] >> 2) | (((uint16_t)a[13*i+ 4] & 0x7f) << 6);
    r->coeffs[8*i+3] = (a[13*i+ 4] >> 7) | (((uint16_t)a[13*i+ 5]       ) << 1) | (((uint16_t)a[13*i+ 6] & 0x0f) <<  9);
    r->coeffs[8*i+4] = (a[13*i+ 6] >> 4) | (((uint16_t)a[13*i+ 7]       ) << 4) | (((uint16_t)a[13*i+ 8] & 0x01) << 12);
    r->coeffs[8*i+5] = (a[13*i+ 8] >> 1) | (((uint16_t)a[13*i+ 9] & 0x3f) << 7);
    r->coeffs[8*i+6] = (a[13*i+ 9] >> 6) | (((uint16_t)a[13*i+10]       ) << 2) | (((uint16_t)a[13*i+11] & 0x07) << 10);
    r->coeffs[8*i+7] = (a[13*i+11] >> 3) | (((uint16_t)a[13*i+12]       ) << 5);
  }
#elif PARAM_Q == 12289
  for(i=0;i<PARAM_N/4;i++)
  {
	  r->coeffs[4*i+0] =  a[7*i+ 0]       | (((uint16_t)a[7*i+ 1] & 0x3f) << 8);
	  r->coeffs[4*i+1] = (a[7*i+ 1] >> 6) | (((uint16_t)a[7*i+ 2]       ) << 2) | (((uint16_t)a[7*i+ 3] & 0x0f) << 10);
	  r->coeffs[4*i+2] = (a[7*i+ 3] >> 4) | (((uint16_t)a[7*i+ 4]       ) << 4) | (((uint16_t)a[7*i+ 5] & 0x03) << 12);
	  r->coeffs[4*i+3] = (a[7*i+ 5] >> 2) | (((uint16_t)a[7*i+ 6]       ) << 6);
  }
#endif
}
void poly_frommsg(poly *r, const unsigned char msg[SEED_BYTES])
{
	int i, j;
	__m128i tmp;
	__m256i a[4], d0, d1, d2, d3;
	const __m256i shift = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
	const __m256i zeros = _mm256_setzero_si256();
	const __m256i ones = _mm256_set1_epi32(1);
	const __m256i hqs = _mm256_set1_epi32((PARAM_Q + 1) / 2);

	for (j = 0; j<SEED_BYTES / 16; j++)
	{
		tmp = _mm_loadu_si128((__m128i *)&msg[j * 16]);
		for (i = 0; i < 4; i++)
		{
			a[i] = _mm256_broadcastd_epi32(tmp);
			tmp = _mm_srli_si128(tmp, 4);
		}

		for (i = 0; i < 4; i++)
		{
			d0 = _mm256_srlv_epi32(a[i], shift);
			d1 = _mm256_srli_epi32(d0, 8);
			d2 = _mm256_srli_epi32(d0, 16);
			d3 = _mm256_srli_epi32(d0, 24);

			d0 = _mm256_and_si256(d0, ones);
			d1 = _mm256_and_si256(d1, ones);
			d2 = _mm256_and_si256(d2, ones);
			d3 = _mm256_and_si256(d3, ones);

			d0 = _mm256_sub_epi32(zeros, d0);
			d1 = _mm256_sub_epi32(zeros, d1);
			d2 = _mm256_sub_epi32(zeros, d2);
			d3 = _mm256_sub_epi32(zeros, d3);

			d0 = _mm256_and_si256(hqs, d0);
			d1 = _mm256_and_si256(hqs, d1);
			d2 = _mm256_and_si256(hqs, d2);
			d3 = _mm256_and_si256(hqs, d3);

			d0 = _mm256_packus_epi32(d0, d1);
			d2 = _mm256_packus_epi32(d2, d3);
			d0 = _mm256_permute4x64_epi64(d0, 0xD8);
			d2 = _mm256_permute4x64_epi64(d2, 0xD8);
			_mm256_store_si256((__m256i *)&r->coeffs[128 * j + 32 * i + 0], d0);
			_mm256_store_si256((__m256i *)&r->coeffs[128 * j + 32 * i + 16], d2);
		}
	}
}
void poly_tomsg(unsigned char msg[SEED_BYTES], const poly *a)
{
	int i, small;
	__m256i vec, tmp;
	const __m256i hqs = _mm256_set1_epi16((PARAM_Q - 1) / 2);
	const __m256i hhqs = _mm256_set1_epi16((PARAM_Q - 5) / 4);

	for (i = 0; i < PARAM_N / 16; i++)
	{
		vec = _mm256_load_si256((__m256i *)&a->coeffs[16 * i]);
		vec = _mm256_sub_epi16(hqs, vec);
		tmp = _mm256_srai_epi16(vec, 15);
		vec = _mm256_xor_si256(vec, tmp);
		vec = _mm256_sub_epi16(hhqs, vec);
		small = _mm256_movemask_epi8(vec);
		small = _pext_u32(small, 0xAAAAAAAA);
		small = ~small;
		msg[2 * i + 0] = small;
		msg[2 * i + 1] = small >> 8;
	}
}
void poly_add(poly *r, const poly *a, const poly *b)
{
  int i;
  __m256i * pa = (__m256i *) a->coeffs;
  __m256i * pb = (__m256i *) b->coeffs;
  __m256i * pr = (__m256i *) r->coeffs;
  __m256i q16x = _mm256_set1_epi16(PARAM_Q);
  
  __m256i t,d;
     
  for(i=0;i<PARAM_N/16;i++)
  {
  	t = _mm256_add_epi16(pa[i],pb[i]);
#if PARAM_Q == 7681
     d = _mm256_srli_epi16(t,13);
#elif PARAM_Q == 12289
     d = _mm256_set1_epi16(5); 
     d = _mm256_mulhi_epu16(t,d);
#endif
     d = _mm256_mullo_epi16(d,q16x);
     
     pr[i] = _mm256_sub_epi16(t,d);
  }
}

void poly_add3(poly *r, const poly *a, const poly *b, const poly *c)
{
  int i;
  __m256i * pa = (__m256i *) a->coeffs;
  __m256i * pb = (__m256i *) b->coeffs;
  __m256i * pc = (__m256i *) c->coeffs;
  __m256i * pr = (__m256i *) r->coeffs;
  __m256i q16x = _mm256_set1_epi16(PARAM_Q);
  
  __m256i t,d;
     
  for(i=0;i<PARAM_N/16;i++)
  {
  	t = _mm256_add_epi16(pa[i],pb[i]);
  	t = _mm256_add_epi16(t,pc[i]);
#if PARAM_Q == 7681
     d = _mm256_srli_epi16(t,13);
#elif PARAM_Q == 12289
     d = _mm256_set1_epi16(5); 
     d = _mm256_mulhi_epu16(t,d);
#endif
     d = _mm256_mullo_epi16(d,q16x);
     
     pr[i] = _mm256_sub_epi16(t,d);
  }
}

void poly_sub(poly *r, const poly *a, const poly *b)
{
  int i;
  __m256i * pa = (__m256i *) a->coeffs;
  __m256i * pb = (__m256i *) b->coeffs;
  __m256i * pr = (__m256i *) r->coeffs;
  __m256i dq16x = _mm256_set1_epi16(2*PARAM_Q);
  __m256i q16x = _mm256_set1_epi16(PARAM_Q);
  
  __m256i t,d;
     
  for(i=0;i<PARAM_N/16;i++)
  {
     t = _mm256_add_epi16(pa[i],dq16x);
  	t = _mm256_sub_epi16(t,pb[i]);
#if PARAM_Q == 7681
     d = _mm256_srli_epi16(t,13);
#elif PARAM_Q == 12289
     d = _mm256_set1_epi16(5); 
     d = _mm256_mulhi_epu16(t,d);
#endif
     d = _mm256_mullo_epi16(d,q16x);
     
     pr[i] = _mm256_sub_epi16(t,d);
  }
}

void poly_ntt(poly *r)
{
  ntt(r->coeffs); 
}
void poly_invntt(poly *r)
{
  invntt(r->coeffs);
}
