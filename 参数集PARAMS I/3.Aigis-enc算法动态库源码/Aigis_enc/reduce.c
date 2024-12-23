#include "reduce.h"
#include "params.h"


#if (PARAM_Q == 7681)
static const uint32_t qinv = 7679; // -inverse_mod(q,2^16)
static const uint32_t barrett_constant = 1; //supportting for reducing an integer in  [0,16*PARAM_Q) to [0,2PARAM_Q)
static const uint32_t barrett_bits = 13;

#elif (PARAM_Q == 12289) /* 256-bit security */
static const uint32_t qinv = 12287; // -inverse_mod(q,2^16)
static const uint32_t barrett_constant = 5; //supportting for reducing an integer in [0,196864) to [0,2PARAM_Q), for this range, we need to use uint32_t in barrett_reduce
static const uint32_t barrett_bits = 16;

#else
#error "reduce.c only supports PARAM_Q in {7681,12289}"
#endif

uint16_t montgomery_reduce(uint32_t a)
{
  uint32_t u;

  u = (a * qinv);
  u &= ((1<<16)-1);
  u *= PARAM_Q;
  
  a = a + u;
  
  return a >> 16;
}
//from [0,16*PARAM_Q) = [0,2*PARAM_Q)
uint16_t barrett_reduce(uint16_t a)
{
  uint16_t u;
#if (PARAM_Q == 7681)
  u = a >> 13;
#elif (PARAM_Q == 12289)
  u = (a*5) >> 16; 
  //u = a >> 14; 
#endif 
  u *= PARAM_Q;
  a -= u;
  
  return a;
}
//from [0,2*PARAM_Q) = [0,PARAM_Q)
uint16_t freeze(uint16_t x)
{
  uint16_t m;
  m = x - PARAM_Q;
  return m ^ ((x^m)&((int16_t)m>>15)); 
}
