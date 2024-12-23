#include "api.h"
#include "params.h"
#include "verify.h"
#include "owcpa.h"
#include "randombytes.h"


int mkem_keygen(unsigned char *pk, unsigned char *sk)
{
  owcpa_keypair(pk, sk);

#ifndef USE_NTT_SK
  memcpy(&sk[SHORT_SK_BYTES], pk, PK_BYTES);
#else
  memcpy(&sk[POLYVEC_BYTES], pk, PK_BYTES);
#endif
  Hash(sk+SK_BYTES-2*SEED_BYTES,pk,PK_BYTES);        
  randombytes(sk + SK_BYTES - SEED_BYTES, SEED_BYTES);/* Value z for implicit reject */
  
  return 0;
}

int mkem_enc(unsigned char *pk, unsigned char *ss, unsigned char *ct)
{
  unsigned char  kr[SEED_BYTES];                                        /* Will contain key, coins */
  unsigned char buf[3*SEED_BYTES];                          

  randombytes(buf, SEED_BYTES);
  Hash(buf,buf,SEED_BYTES);                                           /* Don't release system RNG output */

  Hash(buf + SEED_BYTES, pk, PK_BYTES);                               /* Multitarget countermeasure for coins + contributory KEM */

  Hash(kr, buf, 2*SEED_BYTES);

  owcpa_enc(ct, buf, pk, kr);                                         /* encrypt the pre-k using kr */

  Hash(buf+2*SEED_BYTES, ct, CT_BYTES);                               /* overwrite coins in kr with H(c) */

  Hash(ss, buf, 3*SEED_BYTES);                                        /* hash concatenation of pre-k and H(c) to k */
  return 0;
}


int mdkem_enc(unsigned char *pk, unsigned char *rnd,unsigned char *ss, unsigned char *ct)
{
	unsigned char  kr[SEED_BYTES];                                        /* Will contain key, coins */
	unsigned char buf[3 * SEED_BYTES];

	memcpy(buf, rnd, SEED_BYTES);
	Hash(buf, buf, SEED_BYTES);                                           /* Don't release system RNG output */

	Hash(buf + SEED_BYTES, pk, PK_BYTES);                               /* Multitarget countermeasure for coins + contributory KEM */

	Hash(kr, buf, 2 * SEED_BYTES);

	owcpa_enc(ct, buf, pk, kr);                                         /* encrypt the pre-k using kr */

	Hash(buf + 2 * SEED_BYTES, ct, CT_BYTES);                               /* overwrite coins in kr with H(c) */

	Hash(ss, buf, 3 * SEED_BYTES);                                        /* hash concatenation of pre-k and H(c) to k */
	return 0;
}

int mkem_dec(unsigned char *sk, unsigned char *ct, unsigned char *ss)
{
  int fail;
  unsigned char cmp[CT_BYTES];
  unsigned char buf[3*SEED_BYTES];
  unsigned char kr[SEED_BYTES];                                         /* Will contain key, coins, qrom-hash */
#ifndef USE_NTT_SK
  const unsigned char *pk = sk + SHORT_SK_BYTES;
#else
  const unsigned char *pk = sk + POLYVEC_BYTES;
#endif

  owcpa_dec(buf, ct, sk);                                               /*obtaining pre-k*/
  
  memcpy(&buf[SEED_BYTES], &sk[SK_BYTES - 2 * SEED_BYTES], SEED_BYTES); /* Multitarget countermeasure for coins + contributory KEM */
                                                                        /* Save hash by storing H(pk) in sk */

  Hash(kr, buf, 2*SEED_BYTES);
  owcpa_enc(cmp, buf, pk, kr);                                          /* coins are in kr+SEED_BYTES */

  fail = verify(ct, cmp, CT_BYTES);

  Hash(buf+2*SEED_BYTES, ct, CT_BYTES);                                 /* overwrite coins in kr with H(c)  */

  cmov(buf, sk+SK_BYTES-SEED_BYTES, SEED_BYTES, fail);                  /* Overwrite pre-k with z on re-encryption failure */

  Hash(ss, buf, 3*SEED_BYTES);    
  

  return -fail;
}
