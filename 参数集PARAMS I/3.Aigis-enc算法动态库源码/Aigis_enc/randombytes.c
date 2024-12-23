#include "alg.h"
#include<intrin.h>
#include "rng.h"
#include "params.h"
#include "fips202.h"
#include "randombytes.h"


int randombytes(unsigned char * r, unsigned long long r_byts)
{
	return rand_byts(r_byts, r);
}