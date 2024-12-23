#include <immintrin.h>
#include <string.h>
#include "genmatrix.h"
#include "polyvec.h"
#include "fips202.h"
#include "fips202x4.h"
#include "aes256ctr.h"

/* Generate entry a_{i,j} of matrix A as Parse(SHAKE128(seed|i|j)) */
#if PARAM_Q == 7681
#define NBLOCKS 4
#elif PARAM_Q == 12289
#define NBLOCKS 9
#endif

static const unsigned char idx[256][8] = {
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 2,  0,  0,  0,  0,  0,  0,  0},
  { 0,  2,  0,  0,  0,  0,  0,  0},
  { 4,  0,  0,  0,  0,  0,  0,  0},
  { 0,  4,  0,  0,  0,  0,  0,  0},
  { 2,  4,  0,  0,  0,  0,  0,  0},
  { 0,  2,  4,  0,  0,  0,  0,  0},
  { 6,  0,  0,  0,  0,  0,  0,  0},
  { 0,  6,  0,  0,  0,  0,  0,  0},
  { 2,  6,  0,  0,  0,  0,  0,  0},
  { 0,  2,  6,  0,  0,  0,  0,  0},
  { 4,  6,  0,  0,  0,  0,  0,  0},
  { 0,  4,  6,  0,  0,  0,  0,  0},
  { 2,  4,  6,  0,  0,  0,  0,  0},
  { 0,  2,  4,  6,  0,  0,  0,  0},
  { 8,  0,  0,  0,  0,  0,  0,  0},
  { 0,  8,  0,  0,  0,  0,  0,  0},
  { 2,  8,  0,  0,  0,  0,  0,  0},
  { 0,  2,  8,  0,  0,  0,  0,  0},
  { 4,  8,  0,  0,  0,  0,  0,  0},
  { 0,  4,  8,  0,  0,  0,  0,  0},
  { 2,  4,  8,  0,  0,  0,  0,  0},
  { 0,  2,  4,  8,  0,  0,  0,  0},
  { 6,  8,  0,  0,  0,  0,  0,  0},
  { 0,  6,  8,  0,  0,  0,  0,  0},
  { 2,  6,  8,  0,  0,  0,  0,  0},
  { 0,  2,  6,  8,  0,  0,  0,  0},
  { 4,  6,  8,  0,  0,  0,  0,  0},
  { 0,  4,  6,  8,  0,  0,  0,  0},
  { 2,  4,  6,  8,  0,  0,  0,  0},
  { 0,  2,  4,  6,  8,  0,  0,  0},
  {10,  0,  0,  0,  0,  0,  0,  0},
  { 0, 10,  0,  0,  0,  0,  0,  0},
  { 2, 10,  0,  0,  0,  0,  0,  0},
  { 0,  2, 10,  0,  0,  0,  0,  0},
  { 4, 10,  0,  0,  0,  0,  0,  0},
  { 0,  4, 10,  0,  0,  0,  0,  0},
  { 2,  4, 10,  0,  0,  0,  0,  0},
  { 0,  2,  4, 10,  0,  0,  0,  0},
  { 6, 10,  0,  0,  0,  0,  0,  0},
  { 0,  6, 10,  0,  0,  0,  0,  0},
  { 2,  6, 10,  0,  0,  0,  0,  0},
  { 0,  2,  6, 10,  0,  0,  0,  0},
  { 4,  6, 10,  0,  0,  0,  0,  0},
  { 0,  4,  6, 10,  0,  0,  0,  0},
  { 2,  4,  6, 10,  0,  0,  0,  0},
  { 0,  2,  4,  6, 10,  0,  0,  0},
  { 8, 10,  0,  0,  0,  0,  0,  0},
  { 0,  8, 10,  0,  0,  0,  0,  0},
  { 2,  8, 10,  0,  0,  0,  0,  0},
  { 0,  2,  8, 10,  0,  0,  0,  0},
  { 4,  8, 10,  0,  0,  0,  0,  0},
  { 0,  4,  8, 10,  0,  0,  0,  0},
  { 2,  4,  8, 10,  0,  0,  0,  0},
  { 0,  2,  4,  8, 10,  0,  0,  0},
  { 6,  8, 10,  0,  0,  0,  0,  0},
  { 0,  6,  8, 10,  0,  0,  0,  0},
  { 2,  6,  8, 10,  0,  0,  0,  0},
  { 0,  2,  6,  8, 10,  0,  0,  0},
  { 4,  6,  8, 10,  0,  0,  0,  0},
  { 0,  4,  6,  8, 10,  0,  0,  0},
  { 2,  4,  6,  8, 10,  0,  0,  0},
  { 0,  2,  4,  6,  8, 10,  0,  0},
  {12,  0,  0,  0,  0,  0,  0,  0},
  { 0, 12,  0,  0,  0,  0,  0,  0},
  { 2, 12,  0,  0,  0,  0,  0,  0},
  { 0,  2, 12,  0,  0,  0,  0,  0},
  { 4, 12,  0,  0,  0,  0,  0,  0},
  { 0,  4, 12,  0,  0,  0,  0,  0},
  { 2,  4, 12,  0,  0,  0,  0,  0},
  { 0,  2,  4, 12,  0,  0,  0,  0},
  { 6, 12,  0,  0,  0,  0,  0,  0},
  { 0,  6, 12,  0,  0,  0,  0,  0},
  { 2,  6, 12,  0,  0,  0,  0,  0},
  { 0,  2,  6, 12,  0,  0,  0,  0},
  { 4,  6, 12,  0,  0,  0,  0,  0},
  { 0,  4,  6, 12,  0,  0,  0,  0},
  { 2,  4,  6, 12,  0,  0,  0,  0},
  { 0,  2,  4,  6, 12,  0,  0,  0},
  { 8, 12,  0,  0,  0,  0,  0,  0},
  { 0,  8, 12,  0,  0,  0,  0,  0},
  { 2,  8, 12,  0,  0,  0,  0,  0},
  { 0,  2,  8, 12,  0,  0,  0,  0},
  { 4,  8, 12,  0,  0,  0,  0,  0},
  { 0,  4,  8, 12,  0,  0,  0,  0},
  { 2,  4,  8, 12,  0,  0,  0,  0},
  { 0,  2,  4,  8, 12,  0,  0,  0},
  { 6,  8, 12,  0,  0,  0,  0,  0},
  { 0,  6,  8, 12,  0,  0,  0,  0},
  { 2,  6,  8, 12,  0,  0,  0,  0},
  { 0,  2,  6,  8, 12,  0,  0,  0},
  { 4,  6,  8, 12,  0,  0,  0,  0},
  { 0,  4,  6,  8, 12,  0,  0,  0},
  { 2,  4,  6,  8, 12,  0,  0,  0},
  { 0,  2,  4,  6,  8, 12,  0,  0},
  {10, 12,  0,  0,  0,  0,  0,  0},
  { 0, 10, 12,  0,  0,  0,  0,  0},
  { 2, 10, 12,  0,  0,  0,  0,  0},
  { 0,  2, 10, 12,  0,  0,  0,  0},
  { 4, 10, 12,  0,  0,  0,  0,  0},
  { 0,  4, 10, 12,  0,  0,  0,  0},
  { 2,  4, 10, 12,  0,  0,  0,  0},
  { 0,  2,  4, 10, 12,  0,  0,  0},
  { 6, 10, 12,  0,  0,  0,  0,  0},
  { 0,  6, 10, 12,  0,  0,  0,  0},
  { 2,  6, 10, 12,  0,  0,  0,  0},
  { 0,  2,  6, 10, 12,  0,  0,  0},
  { 4,  6, 10, 12,  0,  0,  0,  0},
  { 0,  4,  6, 10, 12,  0,  0,  0},
  { 2,  4,  6, 10, 12,  0,  0,  0},
  { 0,  2,  4,  6, 10, 12,  0,  0},
  { 8, 10, 12,  0,  0,  0,  0,  0},
  { 0,  8, 10, 12,  0,  0,  0,  0},
  { 2,  8, 10, 12,  0,  0,  0,  0},
  { 0,  2,  8, 10, 12,  0,  0,  0},
  { 4,  8, 10, 12,  0,  0,  0,  0},
  { 0,  4,  8, 10, 12,  0,  0,  0},
  { 2,  4,  8, 10, 12,  0,  0,  0},
  { 0,  2,  4,  8, 10, 12,  0,  0},
  { 6,  8, 10, 12,  0,  0,  0,  0},
  { 0,  6,  8, 10, 12,  0,  0,  0},
  { 2,  6,  8, 10, 12,  0,  0,  0},
  { 0,  2,  6,  8, 10, 12,  0,  0},
  { 4,  6,  8, 10, 12,  0,  0,  0},
  { 0,  4,  6,  8, 10, 12,  0,  0},
  { 2,  4,  6,  8, 10, 12,  0,  0},
  { 0,  2,  4,  6,  8, 10, 12,  0},
  {14,  0,  0,  0,  0,  0,  0,  0},
  { 0, 14,  0,  0,  0,  0,  0,  0},
  { 2, 14,  0,  0,  0,  0,  0,  0},
  { 0,  2, 14,  0,  0,  0,  0,  0},
  { 4, 14,  0,  0,  0,  0,  0,  0},
  { 0,  4, 14,  0,  0,  0,  0,  0},
  { 2,  4, 14,  0,  0,  0,  0,  0},
  { 0,  2,  4, 14,  0,  0,  0,  0},
  { 6, 14,  0,  0,  0,  0,  0,  0},
  { 0,  6, 14,  0,  0,  0,  0,  0},
  { 2,  6, 14,  0,  0,  0,  0,  0},
  { 0,  2,  6, 14,  0,  0,  0,  0},
  { 4,  6, 14,  0,  0,  0,  0,  0},
  { 0,  4,  6, 14,  0,  0,  0,  0},
  { 2,  4,  6, 14,  0,  0,  0,  0},
  { 0,  2,  4,  6, 14,  0,  0,  0},
  { 8, 14,  0,  0,  0,  0,  0,  0},
  { 0,  8, 14,  0,  0,  0,  0,  0},
  { 2,  8, 14,  0,  0,  0,  0,  0},
  { 0,  2,  8, 14,  0,  0,  0,  0},
  { 4,  8, 14,  0,  0,  0,  0,  0},
  { 0,  4,  8, 14,  0,  0,  0,  0},
  { 2,  4,  8, 14,  0,  0,  0,  0},
  { 0,  2,  4,  8, 14,  0,  0,  0},
  { 6,  8, 14,  0,  0,  0,  0,  0},
  { 0,  6,  8, 14,  0,  0,  0,  0},
  { 2,  6,  8, 14,  0,  0,  0,  0},
  { 0,  2,  6,  8, 14,  0,  0,  0},
  { 4,  6,  8, 14,  0,  0,  0,  0},
  { 0,  4,  6,  8, 14,  0,  0,  0},
  { 2,  4,  6,  8, 14,  0,  0,  0},
  { 0,  2,  4,  6,  8, 14,  0,  0},
  {10, 14,  0,  0,  0,  0,  0,  0},
  { 0, 10, 14,  0,  0,  0,  0,  0},
  { 2, 10, 14,  0,  0,  0,  0,  0},
  { 0,  2, 10, 14,  0,  0,  0,  0},
  { 4, 10, 14,  0,  0,  0,  0,  0},
  { 0,  4, 10, 14,  0,  0,  0,  0},
  { 2,  4, 10, 14,  0,  0,  0,  0},
  { 0,  2,  4, 10, 14,  0,  0,  0},
  { 6, 10, 14,  0,  0,  0,  0,  0},
  { 0,  6, 10, 14,  0,  0,  0,  0},
  { 2,  6, 10, 14,  0,  0,  0,  0},
  { 0,  2,  6, 10, 14,  0,  0,  0},
  { 4,  6, 10, 14,  0,  0,  0,  0},
  { 0,  4,  6, 10, 14,  0,  0,  0},
  { 2,  4,  6, 10, 14,  0,  0,  0},
  { 0,  2,  4,  6, 10, 14,  0,  0},
  { 8, 10, 14,  0,  0,  0,  0,  0},
  { 0,  8, 10, 14,  0,  0,  0,  0},
  { 2,  8, 10, 14,  0,  0,  0,  0},
  { 0,  2,  8, 10, 14,  0,  0,  0},
  { 4,  8, 10, 14,  0,  0,  0,  0},
  { 0,  4,  8, 10, 14,  0,  0,  0},
  { 2,  4,  8, 10, 14,  0,  0,  0},
  { 0,  2,  4,  8, 10, 14,  0,  0},
  { 6,  8, 10, 14,  0,  0,  0,  0},
  { 0,  6,  8, 10, 14,  0,  0,  0},
  { 2,  6,  8, 10, 14,  0,  0,  0},
  { 0,  2,  6,  8, 10, 14,  0,  0},
  { 4,  6,  8, 10, 14,  0,  0,  0},
  { 0,  4,  6,  8, 10, 14,  0,  0},
  { 2,  4,  6,  8, 10, 14,  0,  0},
  { 0,  2,  4,  6,  8, 10, 14,  0},
  {12, 14,  0,  0,  0,  0,  0,  0},
  { 0, 12, 14,  0,  0,  0,  0,  0},
  { 2, 12, 14,  0,  0,  0,  0,  0},
  { 0,  2, 12, 14,  0,  0,  0,  0},
  { 4, 12, 14,  0,  0,  0,  0,  0},
  { 0,  4, 12, 14,  0,  0,  0,  0},
  { 2,  4, 12, 14,  0,  0,  0,  0},
  { 0,  2,  4, 12, 14,  0,  0,  0},
  { 6, 12, 14,  0,  0,  0,  0,  0},
  { 0,  6, 12, 14,  0,  0,  0,  0},
  { 2,  6, 12, 14,  0,  0,  0,  0},
  { 0,  2,  6, 12, 14,  0,  0,  0},
  { 4,  6, 12, 14,  0,  0,  0,  0},
  { 0,  4,  6, 12, 14,  0,  0,  0},
  { 2,  4,  6, 12, 14,  0,  0,  0},
  { 0,  2,  4,  6, 12, 14,  0,  0},
  { 8, 12, 14,  0,  0,  0,  0,  0},
  { 0,  8, 12, 14,  0,  0,  0,  0},
  { 2,  8, 12, 14,  0,  0,  0,  0},
  { 0,  2,  8, 12, 14,  0,  0,  0},
  { 4,  8, 12, 14,  0,  0,  0,  0},
  { 0,  4,  8, 12, 14,  0,  0,  0},
  { 2,  4,  8, 12, 14,  0,  0,  0},
  { 0,  2,  4,  8, 12, 14,  0,  0},
  { 6,  8, 12, 14,  0,  0,  0,  0},
  { 0,  6,  8, 12, 14,  0,  0,  0},
  { 2,  6,  8, 12, 14,  0,  0,  0},
  { 0,  2,  6,  8, 12, 14,  0,  0},
  { 4,  6,  8, 12, 14,  0,  0,  0},
  { 0,  4,  6,  8, 12, 14,  0,  0},
  { 2,  4,  6,  8, 12, 14,  0,  0},
  { 0,  2,  4,  6,  8, 12, 14,  0},
  {10, 12, 14,  0,  0,  0,  0,  0},
  { 0, 10, 12, 14,  0,  0,  0,  0},
  { 2, 10, 12, 14,  0,  0,  0,  0},
  { 0,  2, 10, 12, 14,  0,  0,  0},
  { 4, 10, 12, 14,  0,  0,  0,  0},
  { 0,  4, 10, 12, 14,  0,  0,  0},
  { 2,  4, 10, 12, 14,  0,  0,  0},
  { 0,  2,  4, 10, 12, 14,  0,  0},
  { 6, 10, 12, 14,  0,  0,  0,  0},
  { 0,  6, 10, 12, 14,  0,  0,  0},
  { 2,  6, 10, 12, 14,  0,  0,  0},
  { 0,  2,  6, 10, 12, 14,  0,  0},
  { 4,  6, 10, 12, 14,  0,  0,  0},
  { 0,  4,  6, 10, 12, 14,  0,  0},
  { 2,  4,  6, 10, 12, 14,  0,  0},
  { 0,  2,  4,  6, 10, 12, 14,  0},
  { 8, 10, 12, 14,  0,  0,  0,  0},
  { 0,  8, 10, 12, 14,  0,  0,  0},
  { 2,  8, 10, 12, 14,  0,  0,  0},
  { 0,  2,  8, 10, 12, 14,  0,  0},
  { 4,  8, 10, 12, 14,  0,  0,  0},
  { 0,  4,  8, 10, 12, 14,  0,  0},
  { 2,  4,  8, 10, 12, 14,  0,  0},
  { 0,  2,  4,  8, 10, 12, 14,  0},
  { 6,  8, 10, 12, 14,  0,  0,  0},
  { 0,  6,  8, 10, 12, 14,  0,  0},
  { 2,  6,  8, 10, 12, 14,  0,  0},
  { 0,  2,  6,  8, 10, 12, 14,  0},
  { 4,  6,  8, 10, 12, 14,  0,  0},
  { 0,  4,  6,  8, 10, 12, 14,  0},
  { 2,  4,  6,  8, 10, 12, 14,  0},
  { 0,  2,  4,  6,  8, 10, 12, 14}
};
const unsigned int popcount[256] = {
0,1,1,2,1,2,2,3,
1,2,2,3,2,3,3,4,
1,2,2,3,2,3,3,4,
2,3,3,4,3,4,4,5,
1,2,2,3,2,3,3,4,
2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
1,2,2,3,2,3,3,4,
2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,
1,2,2,3,2,3,3,4,
2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,
2,3,3,4,3,4,4,5,
3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,
3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,
4,5,5,6,5,6,6,7,
5,6,6,7,6,7,7,8
};
static int rej_sample(uint16_t *r, size_t rlen, const unsigned char *buf, size_t buflen)
{
	unsigned int ctr, pos;
	uint16_t val;
	uint32_t good;
	const __m256i q16x = _mm256_set1_epi16(PARAM_Q);
#if PARAM_Q == 7681
	const __m256i mask16 = _mm256_set1_epi16(0x1fff);
#elif PARAM_Q == 12289
	const __m256i mask16 = _mm256_set1_epi16(0x3fff);
#endif
	const __m256i ones = _mm256_set1_epi8(1);
	__m256i d0, tmp0, pi0;
	__m128i d, tmp, pilo, pihi;

	ctr = pos = 0;
	while (ctr + 16 < rlen)
	{
		d0 = _mm256_loadu_si256((__m256i *)&buf[pos]);
		d0 = _mm256_and_si256(d0, mask16);
		tmp0 = _mm256_cmpgt_epi16(q16x, d0);
		good = _mm256_movemask_epi8(tmp0);
		good = _pext_u32(good, 0x55555555);
		pilo = _mm_loadl_epi64((__m128i *)&idx[good & 0xFF]);
		pihi = _mm_loadl_epi64((__m128i *)&idx[(good >> 8) & 0xFF]);
		pi0 = _mm256_castsi128_si256(pilo);
		pi0 = _mm256_inserti128_si256(pi0, pihi, 1);

		tmp0 = _mm256_add_epi8(pi0, ones);
		pi0 = _mm256_unpacklo_epi8(pi0, tmp0);

		d0 = _mm256_shuffle_epi8(d0, pi0);

		_mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(d0));
		ctr += popcount[good & 0xFF];
		_mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(d0, 1));
		ctr += popcount[(good >> 8) & 0xFF];

		pos += 32;

		if (pos > buflen - 32)
			return ctr;
	}

	while (ctr + 8 < rlen) {
		d = _mm_loadu_si128((__m128i *)&buf[pos]);
		d = _mm_and_si128(d, _mm256_castsi256_si128(mask16));
		tmp = _mm_cmpgt_epi16(_mm256_castsi256_si128(q16x), d);
		good = _mm_movemask_epi8(tmp);
		good = _pext_u32(good, 0x55555555);
		pilo = _mm_loadl_epi64((__m128i *)&idx[good]);
		pihi = _mm_add_epi8(pilo, _mm256_castsi256_si128(ones));
		pilo = _mm_unpacklo_epi8(pilo, pihi);
		d = _mm_shuffle_epi8(d, pilo);

		_mm_storeu_si128((__m128i *)&r[ctr], d);
		ctr += popcount[good & 0xFF];
		pos += 16;

		if (pos > buflen - 16)
			return ctr;
	}
	while (ctr < rlen) {
#if PARAM_Q == 7681
		val = (buf[pos] | ((uint16_t)buf[pos + 1] << 8)) & 0x1fff;
#elif PARAM_Q == 12289
		val = (buf[pos] | ((uint16_t)buf[pos + 1] << 8)) & 0x3fff;
#endif
		pos += 2;

		if (val < PARAM_Q)
			r[ctr++] = val;

		if (pos > buflen - 2)
			return ctr;
	}
	return 0;
}

void genmatrix_ref(polyvec *a, const unsigned char *seed, int transposed) // Not static for benchmarking
{
	unsigned int ctr, t;
	uint8_t buf[SHAKE128_RATE*NBLOCKS];
	int i, j;
	uint64_t state[25]; // SHAKE state
	unsigned char extseed[SEED_BYTES + 2];

	for (i = 0; i < SEED_BYTES; i++)
		extseed[i] = seed[i];

#if PARAM_Q == 7681
	const uint32_t filter = 0x1fff;
#elif PARAM_Q == 12289
	const uint32_t filter = 0x3fff;
#endif

	for (i = 0; i < PARAM_K; i++)
	{
		for (j = 0; j < PARAM_K; j++)
		{
			ctr = 0;
			if (transposed)
			{
				extseed[SEED_BYTES] = i;
				extseed[SEED_BYTES + 1] = j;
			}
			else
			{
				extseed[SEED_BYTES] = j;
				extseed[SEED_BYTES + 1] = i;
			}

			shake128_absorb(state, extseed, SEED_BYTES + 2);
			shake128_squeezeblocks(buf, NBLOCKS, state);

			t = rej_sample(&a[i].vec[j].coeffs[ctr], PARAM_N - ctr, buf, SHAKE128_RATE*NBLOCKS);

			while (t)
			{
				ctr += t;
				shake128_squeezeblocks(buf, 1, state);
				t = rej_sample(&a[i].vec[j].coeffs[ctr], PARAM_N - ctr, buf, SHAKE128_RATE*NBLOCKS);
			}
		}
	}
}


#ifdef USE_AES
void gen_matrix(polyvec *a, const unsigned char *seed, int transposed)
{
	unsigned int i, j, ctr, t;
	unsigned char __declspec(align(32)) buf[128 * (NBLOCKS + 1)];

	aes256ctr_ctx state;
	aes256ctr_init(&state, seed, 0);

	for (i = 0; i < PARAM_K; i++)
	{
		for (j = 0; j < PARAM_K; j++)
		{
			if (transposed)
				aes256ctr_select(&state, (i << 8) + j);
			else
				aes256ctr_select(&state, (j << 8) + i);

			aes256ctr_squeezeblocks(buf, NBLOCKS + 1, &state);

			ctr = 0;
			t = rej_sample(a[i].vec[j].coeffs, PARAM_N, buf, 128 * NBLOCKS);

			while (t)
			{
				ctr += t;
				aes256ctr_squeezeblocks(buf, 1, &state);
				t = rej_sample(&a[i].vec[j].coeffs[ctr], PARAM_N - ctr, buf, 128);
			}
		}


	}
}

#elif (PARAM_K == 2)

/* Generate entry a_{i,j} of matrix A as Parse(SHAKE128(seed|i|j)) */
void gen_matrix(polyvec *a, const unsigned char *seed, int transposed) // Not static for benchmarking in test/speed.c
{
	uint8_t buf[PARAM_K][PARAM_K][SHAKE128_RATE*NBLOCKS];
  unsigned char extseed0[SEED_BYTES+2];
  unsigned char extseed1[SEED_BYTES+2];
  unsigned char extseed2[SEED_BYTES+2];
  unsigned char extseed3[SEED_BYTES+2];

  int i,j;

  for(i=0;i<SEED_BYTES;i++)
  {
    extseed0[i] = seed[i];
    extseed1[i] = seed[i];
    extseed2[i] = seed[i];
    extseed3[i] = seed[i];
  }

  if(transposed)
  {
    extseed0[SEED_BYTES]   = 0;
    extseed0[SEED_BYTES+1] = 0;
    extseed1[SEED_BYTES]   = 0;
    extseed1[SEED_BYTES+1] = 1;
    extseed2[SEED_BYTES]   = 1;
    extseed2[SEED_BYTES+1] = 0;
    extseed3[SEED_BYTES]   = 1;
    extseed3[SEED_BYTES+1] = 1;
  }
  else
  {
    extseed0[SEED_BYTES]   = 0;
    extseed0[SEED_BYTES+1] = 0;
    extseed1[SEED_BYTES]   = 1;
    extseed1[SEED_BYTES+1] = 0;
    extseed2[SEED_BYTES]   = 0;
    extseed2[SEED_BYTES+1] = 1;
    extseed3[SEED_BYTES]   = 1;
    extseed3[SEED_BYTES+1] = 1;
  }
  shake128x4(buf[0][0], buf[0][1], buf[1][0], buf[1][1], SHAKE128_RATE*NBLOCKS,
             extseed0, extseed1, extseed2, extseed3, SEED_BYTES+2);

  for(i=0;i<PARAM_K;i++)
  {
    for(j=0;j<PARAM_K;j++)
    {
		if (rej_sample(&a[i].vec[j].coeffs, PARAM_N, buf[i][j], SHAKE128_RATE*NBLOCKS))
      {
        genmatrix_ref(a, seed, transposed); // slower, but also extremely unlikely
        return;
      }
    }
  }
}

#elif (PARAM_K == 3)

/* Generate entry a_{i,j} of matrix A as Parse(SHAKE128(seed|i|j)) */
void gen_matrix(polyvec *a, const unsigned char *seed, int transposed) // Not static for benchmarking in test/speed.c
{
  uint8_t buf[PARAM_K][PARAM_K][SHAKE128_RATE*NBLOCKS];
  unsigned char extseed0[SEED_BYTES+2];
  unsigned char extseed1[SEED_BYTES+2];
  unsigned char extseed2[SEED_BYTES+2];
  unsigned char extseed3[SEED_BYTES+2];

  int i,j;

  for(i=0;i<SEED_BYTES;i++)
  {
    extseed0[i] = seed[i];
    extseed1[i] = seed[i];
    extseed2[i] = seed[i];
    extseed3[i] = seed[i];
  }

  if(transposed)
  {
    extseed0[SEED_BYTES]   = 0;
    extseed0[SEED_BYTES+1] = 0;
    extseed1[SEED_BYTES]   = 0;
    extseed1[SEED_BYTES+1] = 1;
    extseed2[SEED_BYTES]   = 0;
    extseed2[SEED_BYTES+1] = 2;
    extseed3[SEED_BYTES]   = 1;
    extseed3[SEED_BYTES+1] = 0;
  }
  else
  {
    extseed0[SEED_BYTES]   = 0;
    extseed0[SEED_BYTES+1] = 0;
    extseed1[SEED_BYTES]   = 1;
    extseed1[SEED_BYTES+1] = 0;
    extseed2[SEED_BYTES]   = 2;
    extseed2[SEED_BYTES+1] = 0;
    extseed3[SEED_BYTES]   = 0;
    extseed3[SEED_BYTES+1] = 1;
  }
  shake128x4(buf[0][0], buf[0][1], buf[0][2], buf[1][0], SHAKE128_RATE*NBLOCKS,
             extseed0, extseed1, extseed2, extseed3, SEED_BYTES+2);

  if(transposed)
  {
    extseed0[SEED_BYTES]   = 1;
    extseed0[SEED_BYTES+1] = 1;
    extseed1[SEED_BYTES]   = 1;
    extseed1[SEED_BYTES+1] = 2;
    extseed2[SEED_BYTES]   = 2;
    extseed2[SEED_BYTES+1] = 0;
    extseed3[SEED_BYTES]   = 2;
    extseed3[SEED_BYTES+1] = 1;
  }
  else
  {
    extseed0[SEED_BYTES]   = 1;
    extseed0[SEED_BYTES+1] = 1;
    extseed1[SEED_BYTES]   = 2;
    extseed1[SEED_BYTES+1] = 1;
    extseed2[SEED_BYTES]   = 0;
    extseed2[SEED_BYTES+1] = 2;
    extseed3[SEED_BYTES]   = 1;
    extseed3[SEED_BYTES+1] = 2;
  }

  shake128x4(buf[1][1], buf[1][2], buf[2][0], buf[2][1], SHAKE128_RATE*NBLOCKS,
             extseed0, extseed1, extseed2, extseed3, SEED_BYTES+2);


  extseed0[SEED_BYTES]   = 2;
  extseed0[SEED_BYTES+1] = 2;

  shake128(buf[2][2], SHAKE128_RATE*NBLOCKS, extseed0, SEED_BYTES + 2);


  for(i=0;i<PARAM_K;i++)
  {
    for(j=0;j<PARAM_K;j++)
    {
		if (rej_sample(&a[i].vec[j].coeffs,PARAM_N, buf[i][j], SHAKE128_RATE*NBLOCKS))
      {
        genmatrix_ref(a, seed, transposed); // slower, but also extremely unlikely
        return;
      }
    }
  }
}
#else
#error "genmatrix only supports PARAM_K in {2,3}"
#endif
