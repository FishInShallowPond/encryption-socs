/**************************************************************************
 * Alg.cpp (Version 1.1.1) created on May 12, 2019.
 *************************************************************************/

#include "Alg.h"
#include "api.h"



puchar_byts_t kem_get_pk_byts()
{
	return KEM_PUBLICKEYBYTES;
}

puchar_byts_t kem_get_sk_byts()
{
	return KEM_SECRETKEYBYTES;
}

puchar_byts_t kem_get_ss_byts()
{
	return KEM_BYTES;
}

puchar_byts_t kem_get_ct_byts()
{
	return KEM_CIPHERTEXTBYTES;
}

int kem_keygen(puchar_t pk, puchar_byts_t* pk_byts,
	puchar_t sk, puchar_byts_t* sk_byts)
{
	mkem_keygen(pk, sk);
	*pk_byts = KEM_PUBLICKEYBYTES;
	*sk_byts = KEM_SECRETKEYBYTES;

	return (0);
}

int kem_enc(puchar_t pk, puchar_byts_t pk_byts,
	puchar_t ss, puchar_byts_t* ss_byts, 
	puchar_t ct, puchar_byts_t* ct_byts)
{
	mkem_enc(pk,ss,ct);
	*ss_byts = KEM_BYTES;
	*ct_byts = KEM_CIPHERTEXTBYTES;
	return (0);
}

int kem_dec(puchar_t sk, puchar_byts_t sk_byts,
	puchar_t ct, puchar_byts_t ct_byts,
	puchar_t ss, puchar_byts_t* ss_byts)
{
	*ss_byts = KEM_BYTES;
	return mkem_dec(sk,ct,ss);
}