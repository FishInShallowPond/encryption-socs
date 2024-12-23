#include "cbd.h"
#include <immintrin.h>
#include<stdio.h>

/*
void cbd1(poly  *r, const unsigned char *buf)
{
	uint32_t d;
	uint16_t a[4], b[4];
	int i;
	for(i=0;i<PARAM_N/4;i++)
	{
		d = (uint32_t)buf[i];

		a[0] = d & 0x1;
		b[0] = (d >> 1) & 0x1;
		a[1] = (d >> 2) & 0x1;
		b[1] = (d >> 3) & 0x1;
		a[2] = (d >> 4) & 0x1;
		b[2] = (d >> 5) & 0x1;
		a[3] = (d >> 6) & 0x1;
		b[3] = (d >> 7) & 0x1;

		r->coeffs[4*i+0] = a[0] + PARAM_Q - b[0];
		r->coeffs[4*i+1] = a[1] + PARAM_Q - b[1];
		r->coeffs[4*i+2] = a[2] + PARAM_Q - b[2];
		r->coeffs[4*i+3] = a[3] + PARAM_Q - b[3];
	}
}*/

const uint16_t tb[4] = { PARAM_Q, PARAM_Q - 1, PARAM_Q + 1, PARAM_Q };
void cbd1(poly  *r, const unsigned char *buf)
{
	int i;
	for (i = 0; i < PARAM_N / 4; i++)
	{
		r->coeffs[4 * i + 0] = tb[buf[i] & 0x3];
		r->coeffs[4 * i + 1] = tb[(buf[i] >> 2) & 0x3];
		r->coeffs[4 * i + 2] = tb[(buf[i] >> 4) & 0x3];
		r->coeffs[4 * i + 3] = tb[buf[i] >> 6];
	}
}
void cbd2(poly  *r, const unsigned char *buf)
{
	uint32_t t, d;
	uint16_t a[2], b[2];
	int i;
	for(i=0;i<PARAM_N/2;i++)
	{
		t = (uint32_t)buf[i];

		d = t & 0x55;
		d += (t>>1) & 0x55; 

		a[0] = d & 0x3;
		b[0] = (d >> 2) & 0x3;
		a[1] = (d >> 4) & 0x3;
		b[1] = (d >> 6) & 0x3;

		r->coeffs[2*i] = a[0] + PARAM_Q - b[0];
		r->coeffs[2*i+1] = a[1] + PARAM_Q - b[1];
	}
}
void cbd4(poly *r, const unsigned char *buf)
{
	int i;
	__m256i * pbuf = (__m256i *) buf;
     __m256i * pr = (__m256i *) r->coeffs;
     
	__m256i mask55 = _mm256_set1_epi8(0x55);
     __m256i mask33 = _mm256_set1_epi8(0x33);
     __m256i mask0f = _mm256_set1_epi8(0x0f);
     __m256i q16x = _mm256_set1_epi16(PARAM_Q);
     
     __m256i  t,d;
     __m128i *pd = (__m128i *) &d;
     
     for(i=0;i<PARAM_N/32;i++)
	{
          d = _mm256_and_si256(pbuf[i],mask55);
          t = _mm256_srli_epi16(pbuf[i],1);
          t = _mm256_and_si256(t,mask55);
          t = _mm256_add_epi8(d,t);
          
          d = _mm256_and_si256(t,mask33);
          t = _mm256_srli_epi16(t,2);
          t = _mm256_and_si256(t,mask33);
          d = _mm256_add_epi8(d,t);


          t = _mm256_and_si256(d,mask0f);
          d = _mm256_srli_epi16(d,4);
          d = _mm256_and_si256(d,mask0f);
          
          d = _mm256_sub_epi8(t,d);
          t = _mm256_cvtepi8_epi16(*pd);
          pr[2*i] = _mm256_add_epi16(t,q16x);
          
          d = _mm256_permute2x128_si256(d,d,0x21);
          t = _mm256_cvtepi8_epi16(*pd);
          pr[2*i+1] = _mm256_add_epi16(t,q16x);
	}
}
void cbd8(poly *r, const unsigned char *buf)
{
	int i;
	__m256i * pbuf = (__m256i *) buf;
     __m256i * pr = (__m256i *) r->coeffs;
     
	__m256i mask55 = _mm256_set1_epi8(0x55);
     __m256i mask33 = _mm256_set1_epi8(0x33);
     __m256i mask0f = _mm256_set1_epi8(0x0f);
     __m256i maskff = _mm256_set1_epi16(0x00ff);
     __m256i q16x = _mm256_set1_epi16(PARAM_Q);
     
     __m256i  t,d;
     
     for(i=0;i<PARAM_N/16;i++)
	{
          d = _mm256_and_si256(pbuf[i],mask55);
          t = _mm256_srli_epi16(pbuf[i],1);
          t = _mm256_and_si256(t,mask55);
          t = _mm256_add_epi16(d,t);
          
          d = _mm256_and_si256(t,mask33);
          t = _mm256_srli_epi16(t,2);
          t = _mm256_and_si256(t,mask33);
          t = _mm256_add_epi16(d,t);
          
          
          d = _mm256_and_si256(t,mask0f);
          t = _mm256_srli_epi16(t,4);
          t = _mm256_and_si256(t,mask0f);
          d = _mm256_add_epi16(d,t);


          t = _mm256_and_si256(d,maskff);
          d = _mm256_srli_epi16(d,8);
          
          d = _mm256_sub_epi16(t,d);
          pr[i] = _mm256_add_epi16(d,q16x);
	}
}
void cbd12(poly  *r, const unsigned char *buf)
{
	int i;
	__m256i * pbuf = (__m256i *) buf;
     __m256i * pr = (__m256i *) r->coeffs;
     
	__m256i mask55 = _mm256_set1_epi8(0x55);
     __m256i mask33 = _mm256_set1_epi8(0x33);
     __m256i mask0f = _mm256_set1_epi8(0x0f);
     __m256i maskff = _mm256_set1_epi16(0xff);
     __m256i q16x = _mm256_set1_epi16(PARAM_Q);
     
     __m256i  t,d,t0,t1;
     __m128i *pd = (__m128i *) &d;
     
     for(i=0;i<PARAM_N/32;i++)
	{
          d = _mm256_and_si256(pbuf[2*i+0],mask55);
          t = _mm256_srli_epi16(pbuf[2*i+0],1);
          t = _mm256_and_si256(t,mask55);
          t = _mm256_add_epi8(d,t);
          
          d = _mm256_and_si256(t,mask33);
          t = _mm256_srli_epi16(t,2);
          t = _mm256_and_si256(t,mask33);
          t = _mm256_add_epi8(d,t);
          
          
          d = _mm256_and_si256(t,mask0f);
          t = _mm256_srli_epi16(t,4);
          t = _mm256_and_si256(t,mask0f);
          d = _mm256_add_epi8(d,t);


          t = _mm256_and_si256(d,maskff);
          d = _mm256_srli_epi16(d,8);
          t0 = _mm256_sub_epi16(t,d);
          
          
          d = _mm256_and_si256(pbuf[2*i+1],mask55);
          t = _mm256_srli_epi16(pbuf[2*i+1],1);
          t = _mm256_and_si256(t,mask55);
          t = _mm256_add_epi8(d,t);
          
          d = _mm256_and_si256(t,mask33);
          t = _mm256_srli_epi16(t,2);
          t = _mm256_and_si256(t,mask33);
          t = _mm256_add_epi8(d,t);
          
          
          d = _mm256_and_si256(t,mask0f);
          t = _mm256_srli_epi16(t,4);
          t = _mm256_and_si256(t,mask0f);
          d = _mm256_add_epi8(d,t);


          t = _mm256_and_si256(d,maskff);
          d = _mm256_srli_epi16(d,8);
          t1 = _mm256_sub_epi16(t,d);
          
          
          d = _mm256_and_si256(pbuf[PARAM_N/16+i],mask55);
          t = _mm256_srli_epi16(pbuf[PARAM_N/16+i],1);
          t = _mm256_and_si256(t,mask55);
          t = _mm256_add_epi8(d,t);
          
          d = _mm256_and_si256(t,mask33);
          t = _mm256_srli_epi16(t,2);
          t = _mm256_and_si256(t,mask33);
          d = _mm256_add_epi8(d,t);


          t = _mm256_and_si256(d,mask0f);
          d = _mm256_srli_epi16(d,4);
          d = _mm256_and_si256(d,mask0f);
          
          d = _mm256_sub_epi8(t,d);
          t = _mm256_cvtepi8_epi16(*pd);
          
          t = _mm256_add_epi16(t,t0);
          pr[2*i] = _mm256_add_epi16(t,q16x);
          
          d = _mm256_permute2x128_si256(d,d,0x21);
          t = _mm256_cvtepi8_epi16(*pd);
          
          t = _mm256_add_epi16(t,t1);
          pr[2*i+1] = _mm256_add_epi16(t,q16x);
	}
}

/*************************************************
* Name:        cbdx
* 
* Description: Given an array of uniformly random bytes, compute 
*              polynomial with coefficients distributed according to
*              a centered binomial distribution with parameter x
*
* Arguments:   - poly *r:                  pointer to output polynomial  
*              - const unsigned char *buf: pointer to input byte array
**************************************************/
void cbdx(poly *r, const unsigned char *buf,uint16_t eta)
{
	switch(eta)
	{
	case 1:
		cbd1(r,buf);
		break;
	case 2:
		cbd2(r,buf);
		break;
	case 4:
		cbd4(r,buf);
		break;
	case 8:
		cbd8(r,buf);
		break;
	case 12:
		cbd12(r,buf);
		break;
	default:
		printf("Error: cbdx only supports eta in {1,2,4,8,12}!\n");
	}
}

void cbd_etas(poly *r, const unsigned char *buf)
{
#if ETA_S == 1
	cbd1(r, buf);
#elif ETA_S == 2
	cbd2(r, buf);
#elif ETA_S == 4
	cbd4(r, buf);
#elif ETA_S == 8
	cbd8(r, buf);
#elif ETA_S == 12
	cbd12(r, buf);
#else
	printf("Error: cbdx only supports eta in {1,2,4,8,12}!\n");
#endif
}

void cbd_etae(poly *r, const unsigned char *buf)
{
#if ETA_E == 1
	cbd1(r, buf);
#elif ETA_E == 2
	cbd2(r, buf);
#elif ETA_E == 4
	cbd4(r, buf);
#elif ETA_E == 8
	cbd8(r, buf);
#elif ETA_E == 12
	cbd12(r, buf);
#else
	printf("Error: cbdx only supports eta in {1,2,4,8,12}!\n");
#endif
}
