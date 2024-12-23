#include <string.h>
#include "owcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "randombytes.h"
#include "fips202.h"
#include "ntt.h"
#include "genmatrix.h"
#include "api.h"
#include "aes256ctr.h"
#include "cbd.h"

static void pack_pk(unsigned char *r, const polyvec *pk, const unsigned char *seed)
{
  int i;
  polyvec_compress(r, pk,BITS_PK);
  for(i=0;i<SEED_BYTES;i++)
    r[PK_POLYVEC_COMPRESSED_BYTES + i] = seed[i];
}


static void unpack_pk(polyvec *pk, unsigned char *seed, const unsigned char *packedpk)
{
  int i;
  polyvec_decompress(pk, packedpk,BITS_PK);

  for(i=0;i<SEED_BYTES;i++)
    seed[i] = packedpk[PK_POLYVEC_COMPRESSED_BYTES +i];
}


static void pack_ciphertext(unsigned char *r, const polyvec *b, const poly *v)
{
  polyvec_compress(r, b,BITS_C1);
  poly_compress(r + CT_POLYVEC_COMPRESSED_BYTES, v);
}

static void unpack_ciphertext(polyvec *b, poly *v, const unsigned char *c)
{
  polyvec_decompress(b, c, BITS_C1);
  poly_decompress(v, c + CT_POLYVEC_COMPRESSED_BYTES);
}

#ifndef USE_NTT_SK
static void pack_sk(unsigned char *r, const polyvec *sk)
{
	unsigned int i, j;
	unsigned char t[8];
#if ETA_S == 1
	for (j = 0; j < PARAM_K; j++)
		for (i = 0; i < PARAM_N / 4; ++i) {
			t[0] = PARAM_Q + ETA_S - sk->vec[j].coeffs[4 * i + 0];
			t[1] = PARAM_Q + ETA_S - sk->vec[j].coeffs[4 * i + 1];
			t[2] = PARAM_Q + ETA_S - sk->vec[j].coeffs[4 * i + 2];
			t[3] = PARAM_Q + ETA_S - sk->vec[j].coeffs[4 * i + 3];
			r[j*PARAM_N / 4 + i] = t[0] | (t[1] << 2) | (t[2] << 4) | (t[3] << 6);
		}
#elif ETA_S == 2
	for (j = 0; j < PARAM_K; j++)
		for (i = 0; i < PARAM_N / 8; ++i) {
			t[0] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 0];
			t[1] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 1];
			t[2] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 2];
			t[3] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 3];
			t[4] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 4];
			t[5] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 5];
			t[6] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 6];
			t[7] = PARAM_Q + ETA_S - sk->vec[j].coeffs[8 * i + 7];

			r[j * 3 * PARAM_N / 8 + 3 * i + 0] = t[0];
			r[j * 3 * PARAM_N / 8 + 3 * i + 0] |= t[1] << 3;
			r[j * 3 * PARAM_N / 8 + 3 * i + 0] |= t[2] << 6;
			r[j * 3 * PARAM_N / 8 + 3 * i + 1] = t[2] >> 2;
			r[j * 3 * PARAM_N / 8 + 3 * i + 1] |= t[3] << 1;
			r[j * 3 * PARAM_N / 8 + 3 * i + 1] |= t[4] << 4;
			r[j * 3 * PARAM_N / 8 + 3 * i + 1] |= t[5] << 7;
			r[j * 3 * PARAM_N / 8 + 3 * i + 2] = t[5] >> 1;
			r[j * 3 * PARAM_N / 8 + 3 * i + 2] |= t[6] << 2;
			r[j * 3 * PARAM_N / 8 + 3 * i + 2] |= t[7] << 5;
		}
#endif
}
#if ETA_S == 1
const uint16_t tb_etas[3] = { PARAM_Q + 1, PARAM_Q, PARAM_Q - 1 };
#elif ETA_S == 2
const uint16_t tb_etas[5] = { PARAM_Q + 2,PARAM_Q + 1, PARAM_Q,PARAM_Q - 1,PARAM_Q - 2 };
#endif
static void unpack_sk(polyvec *sk, const unsigned char *packedsk)
{
	unsigned int i, j, t;
#if ETA_S == 1
	for (j = 0; j < PARAM_K; j++)
		for (i = 0; i < PARAM_N / 4; ++i) {
			t = packedsk[j*PARAM_N / 4 + i] & 0x03;
			sk->vec[j].coeffs[4 * i + 0] = tb_etas[t];
			t = (packedsk[j*PARAM_N / 4 + i] >> 2) & 0x03;
			sk->vec[j].coeffs[4 * i + 1] = tb_etas[t];
			t = (packedsk[j*PARAM_N / 4 + i] >> 4) & 0x03;
			sk->vec[j].coeffs[4 * i + 2] = tb_etas[t];
			t = (packedsk[j*PARAM_N / 4 + i] >> 6);
			sk->vec[j].coeffs[4 * i + 3] = tb_etas[t];
		}
#elif ETA_S == 2
	for (j = 0; j < PARAM_K; j++)
		for (i = 0; i < PARAM_N / 8; ++i) {
			t = packedsk[j * 3 * PARAM_N / 8 + 3 * i + 0] & 0x07;
			sk->vec[j].coeffs[8 * i + 0] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 0] >> 3) & 0x07;
			sk->vec[j].coeffs[8 * i + 1] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 0] >> 6) | ((packedsk[j * 3 * PARAM_N / 8 + 3 * i + 1] & 0x01) << 2);
			sk->vec[j].coeffs[8 * i + 2] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 1] >> 1) & 0x07;
			sk->vec[j].coeffs[8 * i + 3] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 1] >> 4) & 0x07;
			sk->vec[j].coeffs[8 * i + 4] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 1] >> 7) | ((packedsk[j * 3 * PARAM_N / 8 + 3 * i + 2] & 0x03) << 1);
			sk->vec[j].coeffs[8 * i + 5] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 2] >> 2) & 0x07;
			sk->vec[j].coeffs[8 * i + 6] = tb_etas[t];
			t = (packedsk[j * 3 * PARAM_N / 8 + 3 * i + 2] >> 5);
			sk->vec[j].coeffs[8 * i + 7] = tb_etas[t];
		}
#endif
	polyvec_ntt(sk);
}
#else
static void pack_sk(unsigned char *r, const polyvec *sk)
{
	polyvec_tobytes(r, sk);
}

static void unpack_sk(polyvec *sk, const unsigned char *packedsk)
{
	polyvec_frombytes(sk, packedsk);
}
#endif

#define gen_a(A,B)  gen_matrix(A,B,0)
#define gen_at(A,B) gen_matrix(A,B,1)

void owcpa_keypair(unsigned char *pk, 
                   unsigned char *sk)
{
  polyvec a[PARAM_K], e, pkpv, skpv;
  unsigned char buf[SEED_BYTES+SEED_BYTES];
  unsigned char *publicseed = buf;
  unsigned char *noiseseed = buf+SEED_BYTES;
  int i;
  unsigned char nonce=0;

#ifdef USE_AES 
  aes256ctr_ctx state;
  unsigned char __declspec(align(32)) coins[ETA_E*PARAM_N / 4 + 128];
#endif

  randombytes(buf, SEED_BYTES);

  Hash2(buf, buf, SEED_BYTES);

  gen_a(a, publicseed);
 

#ifdef USE_AES 
  aes256ctr_init(&state, noiseseed, 0);
  for (i = 0; i < PARAM_K; i++)
  {
	  aes256ctr_select(&state, (uint16_t)nonce++ << 8);
	  aes256ctr_squeezeblocks(coins, (ETA_S*PARAM_N / 4 + 128) / 128, &state);
	  cbdx(&skpv.vec[i], coins, ETA_S);
  }
  for (i = 0; i < PARAM_K; i++)
  {
	  aes256ctr_select(&state, (uint16_t)nonce++ << 8);
	  aes256ctr_squeezeblocks(coins, (ETA_E*PARAM_N / 4 + 128) / 128, &state);
	  cbdx(&e.vec[i], coins, ETA_E);
  }
#else 

  polyvec_getnoise_etas(&skpv, noiseseed, nonce);
  nonce += PARAM_K;

  polyvec_getnoise_etae(&e, noiseseed, nonce);
  nonce += PARAM_K;

#endif

#ifndef USE_NTT_SK
  pack_sk(sk, &skpv);//store short sk
#endif
  polyvec_ntt(&skpv);

  // matrix-vector multiplication
  for(i=0;i<PARAM_K;i++)
    polyvec_pointwise_acc(&pkpv.vec[i],&skpv,a+i);

  polyvec_invntt(&pkpv);
  polyvec_add(&pkpv,&pkpv,&e);

#ifdef USE_NTT_SK
  pack_sk(sk, &skpv);//store ntt sk
#endif
  pack_pk(pk, &pkpv, publicseed);
  
  
}

void owcpa_enc(unsigned char *c,
               const unsigned char *m,
               const unsigned char *pk,
               const unsigned char *coins)
{
  polyvec sp, pkpv, ep, at[PARAM_K], bp;
  poly v, k, epp;
  unsigned char seed[SEED_BYTES];
  int i;
  unsigned char nonce=0;
#ifdef USE_AES
  aes256ctr_ctx state;
  unsigned char __declspec(align(32))  buf[ETA_E*PARAM_N / 4 + 128];
#endif

  unpack_pk(&pkpv, seed, pk);

  poly_frommsg(&k, m);

  polyvec_ntt(&pkpv);
 
  gen_at(at, seed);

#ifdef USE_AES
  aes256ctr_init(&state, coins, 0);
  for (i = 0; i < PARAM_K; i++)
  {
	  aes256ctr_select(&state, (uint16_t)nonce++ << 8);
	  aes256ctr_squeezeblocks(buf, (ETA_S*PARAM_N / 4 + 128) / 128, &state);
	  cbdx(&sp.vec[i], buf, ETA_S);
  }
  for (i = 0; i < PARAM_K; i++) {
	  aes256ctr_select(&state, (uint16_t)nonce++ << 8);
	  aes256ctr_squeezeblocks(buf, (ETA_E*PARAM_N / 4 + 128) / 128, &state);
	  cbdx(&ep.vec[i], buf, ETA_E);
  }
  aes256ctr_select(&state, (uint16_t)nonce++ << 8);
  aes256ctr_squeezeblocks(buf, (ETA_E*PARAM_N / 4 + 128) / 128, &state);
  cbdx(&epp, buf, ETA_E);
#else
  polyvec_getnoise_etas(&sp, coins, nonce);
  nonce += PARAM_K;
  
#if PARAM_K == 2
  poly_getnoise_etae3x(&ep.vec[0], &ep.vec[1], &epp, coins, nonce, ETA_E);
#elif PARAM_K == 3
  poly_getnoise_etae4x(&ep.vec[0], &ep.vec[1], &ep.vec[2], &epp, coins, nonce);
#endif 

#endif
  polyvec_ntt(&sp);

  // matrix-vector multiplication
  for(i=0;i<PARAM_K;i++)
    polyvec_pointwise_acc(&bp.vec[i],&sp,at+i);

  polyvec_invntt(&bp);
  polyvec_add(&bp, &bp, &ep);
 
  polyvec_pointwise_acc(&v, &pkpv, &sp);
  poly_invntt(&v);


  poly_add3(&v, &v, &epp,&k);

  pack_ciphertext(c, &bp, &v); 
}

void owcpa_dec(unsigned char *m,
               const unsigned char *c,
               const unsigned char *sk)
{
  polyvec bp, skpv;
  poly v, mp;

  unpack_ciphertext(&bp, &v, c);
  unpack_sk(&skpv, sk);

  polyvec_ntt(&bp);

  polyvec_pointwise_acc(&mp,&skpv,&bp);
  poly_invntt(&mp);

  poly_sub(&mp, &mp, &v);

  poly_tomsg(m, &mp);
}
