#include "inttypes.h"
#include <immintrin.h>
#include "ntt.h"
#include "params.h"
#include "reduce.h"

#if PARAM_Q == 7681
void ntt(uint16_t *a)
{
    int i,j;
    __m256i *p256a = (__m256i *) a;
    __m256i *p256zeta = (__m256i *) zetas_avx;  
    __m256i vq16x = _mm256_set1_epi16(PARAM_Q);
    __m256i v2q16x =  _mm256_set1_epi16(2*PARAM_Q);
    __m256i vqinv16x = _mm256_set1_epi16(57857U); //inverse_mod(q,2^16)
    __m256i t[12];
    __m256i idx = _mm256_set_epi32(6,7,4,5,2,3,0,1);
     
    for(i=0; i< 2; i++)
    {
        //level 7
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+8],p256zeta[0]);
        t[4] = _mm256_mulhi_epu16(p256a[i+8],p256zeta[0]);
        t[9] = _mm256_mullo_epi16(p256a[i+10],p256zeta[0]);
        t[5] = _mm256_mulhi_epu16(p256a[i+10],p256zeta[0]);
        t[10] = _mm256_mullo_epi16(p256a[i+12],p256zeta[0]);
        t[6] = _mm256_mulhi_epu16(p256a[i+12],p256zeta[0]);
        t[11] = _mm256_mullo_epi16(p256a[i+14],p256zeta[0]);
        t[7] = _mm256_mulhi_epu16(p256a[i+14],p256zeta[0]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);
        
        
        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[4] =  _mm256_adds_epu16(p256a[i],v2q16x);
        t[5] =  _mm256_adds_epu16(p256a[i+2],v2q16x);
        t[6] =  _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[7] =  _mm256_adds_epu16(p256a[i+6],v2q16x);
        
        t[4] =  _mm256_subs_epu16(t[4],t[8]);
        t[5] =  _mm256_subs_epu16(t[5],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
    
        t[0] =  _mm256_adds_epu16(p256a[i],t[8]);
        t[1] =  _mm256_adds_epu16(p256a[i+2],t[9]);
        t[2] =  _mm256_adds_epu16(p256a[i+4],t[10]);
        t[3] =  _mm256_adds_epu16(p256a[i+6],t[11]);
            
        //level 6
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[1]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[1]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[1]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[1]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[2]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[2]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[2]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[2]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[2] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[1],v2q16x);
        t[6] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[5],v2q16x);
        
        t[2] =  _mm256_subs_epu16(t[2],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[1] =  _mm256_adds_epu16(t[1],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[5] =  _mm256_adds_epu16(t[5],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],13);
        t[9] =  _mm256_srli_epi16(t[3],13);
        t[10] =  _mm256_srli_epi16(t[5],13);
        t[11] =  _mm256_srli_epi16(t[7],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);


        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[0] =  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
       
        //level 5
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[3]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[3]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[4]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[4]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[5]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[5]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[6]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[6]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
  
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
            

        p256a[i+2] =  _mm256_subs_epu16(t[1],t[8]);
        p256a[i+6] =  _mm256_subs_epu16(t[3],t[9]);
        p256a[i+10] =  _mm256_subs_epu16(t[5],t[10]);
        p256a[i+14] =  _mm256_subs_epu16(t[7],t[11]);

        p256a[i+0] =  _mm256_adds_epu16(t[0],t[8]);
        p256a[i+4] =  _mm256_adds_epu16(t[2],t[9]);
        p256a[i+8] =  _mm256_adds_epu16(t[4],t[10]);
        p256a[i+12] =  _mm256_adds_epu16(t[6],t[11]);
    }
    
  
    j=0;
    for(i=0; i< 16; i+= 8,j+=4)
    {
        //level 4
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+1],p256zeta[7+j]);
        t[1] = _mm256_mulhi_epu16(p256a[i+1],p256zeta[7+j]);
        t[9] = _mm256_mullo_epi16(p256a[i+3],p256zeta[8+j]);
        t[3] = _mm256_mulhi_epu16(p256a[i+3],p256zeta[8+j]);
        t[10] = _mm256_mullo_epi16(p256a[i+5],p256zeta[9+j]);
        t[5] = _mm256_mulhi_epu16(p256a[i+5],p256zeta[9+j]);
        t[11] = _mm256_mullo_epi16(p256a[i+7],p256zeta[10+j]);
        t[7] = _mm256_mulhi_epu16(p256a[i+7],p256zeta[10+j]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);


        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(p256a[i+0],v2q16x);
        t[3] = _mm256_adds_epu16(p256a[i+2],v2q16x);
        t[5] = _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[7] = _mm256_adds_epu16(p256a[i+6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
           
        t[0] =  _mm256_adds_epu16(p256a[i+0],t[8]);
        t[2] =  _mm256_adds_epu16(p256a[i+2],t[9]);
        t[4] =  _mm256_adds_epu16(p256a[i+4],t[10]);
        t[6] =  _mm256_adds_epu16(p256a[i+6],t[11]);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[1],13);
        t[9] =  _mm256_srli_epi16(t[3],13);
        t[10] =  _mm256_srli_epi16(t[5],13);
        t[11] =  _mm256_srli_epi16(t[7],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);


        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[8] =  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);
        
  
        //level 3  
        t[0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        t[1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        t[2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        t[3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        t[4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        t[5] = _mm256_permute2x128_si256(t[10],t[5],0x31);
        t[6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        t[7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
        
        
         //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[15+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[15+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[16+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[16+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[17+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[17+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[18+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[18+j]);
         
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);

        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[8] =  _mm256_adds_epu16(t[0],t[8]);
        t[9] =  _mm256_adds_epu16(t[2],t[9]);
        t[10] =  _mm256_adds_epu16(t[4],t[10]);
        t[11] =  _mm256_adds_epu16(t[6],t[11]);
                           
        //level 2   
        t[8] = _mm256_permute4x64_epi64(t[8],0xb1);
        t[9] = _mm256_permute4x64_epi64(t[9],0xb1);
        t[10] = _mm256_permute4x64_epi64(t[10],0xb1);
        t[11] = _mm256_permute4x64_epi64(t[11],0xb1);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xcc);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xcc);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xcc);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xcc);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xcc);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xcc);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xcc);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xcc);
  
        t[0] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[2] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[4] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[6] = _mm256_permute4x64_epi64(t[6],0xb1);
        
        
         //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[23+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[23+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[24+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[24+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[25+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[25+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[26+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[26+j]);
         

        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);

        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
      
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],13);
        t[9] =  _mm256_srli_epi16(t[3],13);
        t[10] =  _mm256_srli_epi16(t[5],13);
        t[11] =  _mm256_srli_epi16(t[7],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);


        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[0] =  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 1  
        t[8] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[9] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[10] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[11] = _mm256_permutevar8x32_epi32(t[6],idx);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xaa);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xaa);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xaa);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xaa);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xaa);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xaa);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xaa);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xaa);
  
        t[0] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[2] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[4] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[6] = _mm256_permutevar8x32_epi32(t[6],idx);
        
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[31+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[31+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[32+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[32+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[33+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[33+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[34+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[34+j]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
      
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        //level 0
        t[8] = _mm256_srli_epi32(t[0],16);
        t[9] = _mm256_srli_epi32(t[2],16); 
        t[10] = _mm256_slli_epi32(t[1],16);
        t[11] = _mm256_slli_epi32(t[3],16);
        
        t[0] = _mm256_blend_epi16(t[0],t[10],0xaa);
        t[1] = _mm256_blend_epi16(t[8],t[1],0xaa);
        t[2] = _mm256_blend_epi16(t[2],t[11],0xaa);
        t[3] = _mm256_blend_epi16(t[9],t[3],0xaa);
         
        t[8] = _mm256_srli_epi32(t[4],16);
        t[9] = _mm256_srli_epi32(t[6],16); 
        t[10] = _mm256_slli_epi32(t[5],16);
        t[11] = _mm256_slli_epi32(t[7],16);
        
        t[4] = _mm256_blend_epi16(t[4],t[10],0xaa);
        t[5] = _mm256_blend_epi16(t[8],t[5],0xaa);
        t[6] = _mm256_blend_epi16(t[6],t[11],0xaa);
        t[7] = _mm256_blend_epi16(t[9],t[7],0xaa);
        
     
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[39+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[39+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[40+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[40+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[41+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[41+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[42+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[42+j]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
      
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],13);
        t[9] =  _mm256_srli_epi16(t[3],13);
        t[10] =  _mm256_srli_epi16(t[5],13);
        t[11] =  _mm256_srli_epi16(t[7],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);


        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[0] =  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
            
        //store   
        t[8] = _mm256_unpacklo_epi16(t[0],t[1]);
        t[1] = _mm256_unpackhi_epi16(t[0],t[1]);
        t[9] = _mm256_unpacklo_epi16(t[2],t[3]);
        t[3] = _mm256_unpackhi_epi16(t[2],t[3]);
        t[10] = _mm256_unpacklo_epi16(t[4],t[5]);
        t[5] = _mm256_unpackhi_epi16(t[4],t[5]);
        t[11] = _mm256_unpacklo_epi16(t[6],t[7]);
        t[7] = _mm256_unpackhi_epi16(t[6],t[7]);
        
        p256a[i+0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        p256a[i+1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        p256a[i+2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        p256a[i+3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        p256a[i+4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        p256a[i+5] = _mm256_permute2x128_si256(t[10],t[5],0x31); 
        p256a[i+6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        p256a[i+7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
    }
}

void invntt(uint16_t a[PARAM_N])
{
    int i,j;
    __m256i *p256a = (__m256i *) a;
    __m256i *p256zeta = (__m256i *) zetas_inv_avx;    
    __m256i vq16x = _mm256_set1_epi16(PARAM_Q);
    __m256i v4q16x =  _mm256_set1_epi16(4*PARAM_Q);
    __m256i vqinv16x = _mm256_set1_epi16(57857U); //inverse_mod(q,2^16)
    __m256i invn16x = _mm256_set1_epi16(256); //invn*2^16 mod q
    __m256i t[16];
    __m256i idx = _mm256_set_epi32(6,7,4,5,2,3,0,1);
    __m256i mask = _mm256_set1_epi32(0xFFFF);
    
    
    j=0;
    for(i=0;i<16;i+=8,j+=4)
    {
        //level 0   
        //pack the data
        t[8] = _mm256_permute2x128_si256(p256a[i+0],p256a[i+1],0x20);
        t[1] = _mm256_permute2x128_si256(p256a[i+0],p256a[i+1],0x31);
        t[9] = _mm256_permute2x128_si256(p256a[i+2],p256a[i+3],0x20);
        t[3] = _mm256_permute2x128_si256(p256a[i+2],p256a[i+3],0x31);
        t[10]= _mm256_permute2x128_si256(p256a[i+4],p256a[i+5],0x20);
        t[5] = _mm256_permute2x128_si256(p256a[i+4],p256a[i+5],0x31); 
        t[11]= _mm256_permute2x128_si256(p256a[i+6],p256a[i+7],0x20);
        t[7] = _mm256_permute2x128_si256(p256a[i+6],p256a[i+7],0x31);

        t[12] = _mm256_and_si256(t[8],mask);
        t[13] = _mm256_and_si256(t[9],mask);
        t[14] = _mm256_and_si256(t[1],mask);
        t[15] = _mm256_and_si256(t[3],mask);
        t[0] = _mm256_packus_epi32(t[12],t[14]);
        t[2] = _mm256_packus_epi32(t[13],t[15]);
        
        t[12] = _mm256_and_si256(t[10],mask);
        t[13] = _mm256_and_si256(t[11],mask);
        t[14] = _mm256_and_si256(t[5],mask);
        t[15] = _mm256_and_si256(t[7],mask);
        t[4] = _mm256_packus_epi32(t[12],t[14]);
        t[6] = _mm256_packus_epi32(t[13],t[15]);
        
        
        t[8] = _mm256_srli_epi32(t[8],16);
        t[1] = _mm256_srli_epi32(t[1],16);
        t[9] = _mm256_srli_epi32(t[9],16);
        t[3] = _mm256_srli_epi32(t[3],16);
        t[10]= _mm256_srli_epi32(t[10],16);
        t[5] = _mm256_srli_epi32(t[5],16);
        t[11]= _mm256_srli_epi32(t[11],16);
        t[7] = _mm256_srli_epi32(t[7],16);

        t[1] = _mm256_packus_epi32(t[8],t[1]);
        t[3] = _mm256_packus_epi32(t[9],t[3]);
        t[5] = _mm256_packus_epi32(t[10],t[5]);
        t[7] = _mm256_packus_epi32(t[11],t[7]);
        
        
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[2],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[6],v4q16x);
        
        t[0]  = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[0+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[0+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[1+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[1+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[2+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[2+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[3+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[3+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //level 1  
        t[8] = _mm256_srli_epi32(t[0],16);
        t[9] = _mm256_srli_epi32(t[2],16); 
        t[10] = _mm256_slli_epi32(t[1],16);
        t[11] = _mm256_slli_epi32(t[3],16);
        
        t[0] = _mm256_blend_epi16(t[0],t[10],0xaa);
        t[1] = _mm256_blend_epi16(t[8],t[1],0xaa);
        t[2] = _mm256_blend_epi16(t[2],t[11],0xaa);
        t[3] = _mm256_blend_epi16(t[9],t[3],0xaa);
         
        t[8] = _mm256_srli_epi32(t[4],16);
        t[9] = _mm256_srli_epi32(t[6],16); 
        t[10] = _mm256_slli_epi32(t[5],16);
        t[11] = _mm256_slli_epi32(t[7],16);
        
        t[4] = _mm256_blend_epi16(t[4],t[10],0xaa);
        t[5] = _mm256_blend_epi16(t[8],t[5],0xaa);
        t[6] = _mm256_blend_epi16(t[6],t[11],0xaa);
        t[7] = _mm256_blend_epi16(t[9],t[7],0xaa);
        
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[2],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[6],v4q16x);
        
        t[0]  = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[8+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[8+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[9+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[9+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[10+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[10+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[11+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[11+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[0] =  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 2
        
        t[8] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[9] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[10] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[11] = _mm256_permutevar8x32_epi32(t[6],idx);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xaa);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xaa);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xaa);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xaa);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xaa);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xaa);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xaa);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xaa);
  
        t[0] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[2] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[4] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[6] = _mm256_permutevar8x32_epi32(t[6],idx);
        
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[2],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[6],v4q16x);
        
        t[0]  = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[16+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[16+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[17+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[17+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[18+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[18+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[19+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[19+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //level 3 
        t[8] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[9] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[10] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[11] = _mm256_permute4x64_epi64(t[6],0xb1);
           
        t[0] = _mm256_blend_epi32(t[1],t[8],0xcc);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xcc);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xcc);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xcc);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xcc);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xcc);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xcc);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xcc);
  
        t[0] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[2] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[4] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[6] = _mm256_permute4x64_epi64(t[6],0xb1);
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[2],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[6],v4q16x);
        
        t[0]  = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[24+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[24+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[25+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[25+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[26+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[26+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[27+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[27+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[8] =  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 4 
        t[0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        t[1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        t[2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        t[3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        t[4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        t[5] = _mm256_permute2x128_si256(t[10],t[5],0x31);
        t[6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        t[7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[2],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[6],v4q16x);
        
        p256a[i+0]  = _mm256_adds_epu16(t[0],t[1]);
        p256a[i+2]  = _mm256_adds_epu16(t[2],t[3]);
        p256a[i+4]  = _mm256_adds_epu16(t[4],t[5]);
        p256a[i+6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[32+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[32+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[33+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[33+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[34+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[34+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[35+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[35+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[1] = _mm256_sub_epi16(t[1],t[8]);
        t[3] = _mm256_sub_epi16(t[3],t[9]);
        t[5] = _mm256_sub_epi16(t[5],t[10]);
        t[7] = _mm256_sub_epi16(t[7],t[11]);
        
        p256a[i+1] = _mm256_add_epi16(t[1],vq16x);
        p256a[i+3] = _mm256_add_epi16(t[3],vq16x);
        p256a[i+5] = _mm256_add_epi16(t[5],vq16x);
        p256a[i+7] = _mm256_add_epi16(t[7],vq16x);
    }    
    for(i=0;i<2;i++)
    {
    	   
    	  //level 5  
       //butterfly 
        t[8]  = _mm256_adds_epu16(p256a[i+0],v4q16x);
        t[9]  = _mm256_adds_epu16(p256a[i+4],v4q16x);
        t[10]  = _mm256_adds_epu16(p256a[i+8],v4q16x);
        t[11]  = _mm256_adds_epu16(p256a[i+12],v4q16x);
        
        
        t[0]  = _mm256_adds_epu16(p256a[i+0],p256a[i+2]);
        t[2]  = _mm256_adds_epu16(p256a[i+4],p256a[i+6]);
        t[4]  = _mm256_adds_epu16(p256a[i+8],p256a[i+10]);
        t[6]  = _mm256_adds_epu16(p256a[i+12],p256a[i+14]);
           
        t[1]  = _mm256_subs_epu16(t[8],p256a[i+2]);
        t[3]  = _mm256_subs_epu16(t[9],p256a[i+6]);
        t[5]  = _mm256_subs_epu16(t[10],p256a[i+10]);
        t[7]  = _mm256_subs_epu16(t[11],p256a[i+14]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[40]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[40]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[41]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[41]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[42]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[42]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[43]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[43]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],13);
        t[9] =  _mm256_srli_epi16(t[2],13);
        t[10] =  _mm256_srli_epi16(t[4],13);
        t[11] =  _mm256_srli_epi16(t[6],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        t[0] =  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
         
    	  //level 6
    	   //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[1],v4q16x);
        t[10]  = _mm256_adds_epu16(t[4],v4q16x);
        t[11]  = _mm256_adds_epu16(t[5],v4q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[2]);
        t[1]  = _mm256_adds_epu16(t[1],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[6]);
        t[5]  = _mm256_adds_epu16(t[5],t[7]);
           
        t[2]  = _mm256_subs_epu16(t[8],t[2]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[44]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[44]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[44]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[44]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[45]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[45]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[45]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[45]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[2] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[6] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
    	  
    	  
    	  //level 7
    	  
    	  //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v4q16x);
        t[9]  = _mm256_adds_epu16(t[1],v4q16x);
        t[10]  = _mm256_adds_epu16(t[2],v4q16x);
        t[11]  = _mm256_adds_epu16(t[3],v4q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[4]);
        t[1]  = _mm256_adds_epu16(t[1],t[5]);
        t[2]  = _mm256_adds_epu16(t[2],t[6]);
        t[3]  = _mm256_adds_epu16(t[3],t[7]);
           
        t[4]  = _mm256_subs_epu16(t[8],t[4]);
        t[5]  = _mm256_subs_epu16(t[9],t[5]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[4],p256zeta[46]);
        t[4] = _mm256_mulhi_epu16(t[4],p256zeta[46]);
        t[9] = _mm256_mullo_epi16(t[5],p256zeta[46]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[46]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[46]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[46]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[46]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[46]);
        
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[4] = _mm256_add_epi16(t[8],vq16x);
        t[5] = _mm256_add_epi16(t[9],vq16x);
        t[6] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        
        //mul invn
        t[8] = _mm256_mullo_epi16(t[0],invn16x);
        t[0] = _mm256_mulhi_epu16(t[0],invn16x);
        t[9] = _mm256_mullo_epi16(t[1],invn16x);
        t[1] = _mm256_mulhi_epu16(t[1],invn16x);
        t[10] = _mm256_mullo_epi16(t[2],invn16x);
        t[2] = _mm256_mulhi_epu16(t[2],invn16x);
        t[11] = _mm256_mullo_epi16(t[3],invn16x);
        t[3] = _mm256_mulhi_epu16(t[3],invn16x);
         
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[0],t[8]);
        t[9] = _mm256_sub_epi16(t[1],t[9]);
        t[10] = _mm256_sub_epi16(t[2],t[10]);
        t[11] = _mm256_sub_epi16(t[3],t[11]);
        
        p256a[i+0] = _mm256_add_epi16(t[8],vq16x);
        p256a[i+2] = _mm256_add_epi16(t[9],vq16x);
        p256a[i+4] = _mm256_add_epi16(t[10],vq16x);
        p256a[i+6] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[4],13);
        t[9] =  _mm256_srli_epi16(t[5],13);
        t[10] =  _mm256_srli_epi16(t[6],13);
        t[11] =  _mm256_srli_epi16(t[7],13);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] =  _mm256_mullo_epi16(t[10],vq16x);
        t[11] =  _mm256_mullo_epi16(t[11],vq16x);
        
        p256a[i+8] =  _mm256_sub_epi16(t[4],t[8]);
        p256a[i+10] =  _mm256_sub_epi16(t[5],t[9]);
        p256a[i+12] =  _mm256_sub_epi16(t[6],t[10]);
        p256a[i+14] =  _mm256_sub_epi16(t[7],t[11]);  	
    }
    
}
#elif PARAM_Q == 12289
void ntt(uint16_t a[PARAM_N])
{
    int i,j;
    __m256i *p256a = (__m256i *) a;
    __m256i *p256zeta = (__m256i *) zetas_avx; 
    __m256i vq16x = _mm256_set1_epi16(PARAM_Q);
    __m256i v2q16x =  _mm256_set1_epi16(2*PARAM_Q);
    __m256i vqinv16x = _mm256_set1_epi16(53249U); //inverse_mod(q,2^16)
    __m256i t[12];
    __m256i idx = _mm256_set_epi32(6,7,4,5,2,3,0,1);
    
    
    for(i=0; i< 4; i++)
    {
        //level 8
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+16],p256zeta[0]);
        t[4] = _mm256_mulhi_epu16(p256a[i+16],p256zeta[0]);
        t[9] = _mm256_mullo_epi16(p256a[i+20],p256zeta[0]);
        t[5] = _mm256_mulhi_epu16(p256a[i+20],p256zeta[0]);
        t[10] = _mm256_mullo_epi16(p256a[i+24],p256zeta[0]);
        t[6] = _mm256_mulhi_epu16(p256a[i+24],p256zeta[0]);
        t[11] = _mm256_mullo_epi16(p256a[i+28],p256zeta[0]);
        t[7] = _mm256_mulhi_epu16(p256a[i+28],p256zeta[0]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);
        
        
        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[4] =  _mm256_adds_epu16(p256a[i],v2q16x);
        t[5] =  _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[6] =  _mm256_adds_epu16(p256a[i+8],v2q16x);
        t[7] =  _mm256_adds_epu16(p256a[i+12],v2q16x);
        
        t[4] =  _mm256_subs_epu16(t[4],t[8]);
        t[5] =  _mm256_subs_epu16(t[5],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
    
        t[0] =  _mm256_adds_epu16(p256a[i],t[8]);
        t[1] =  _mm256_adds_epu16(p256a[i+4],t[9]);
        t[2] =  _mm256_adds_epu16(p256a[i+8],t[10]);
        t[3] =  _mm256_adds_epu16(p256a[i+12],t[11]);
        
        // reduce to [0,2*Q)   
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[1],14);
        t[10] = _mm256_srli_epi16(t[2],14);
        t[11] = _mm256_srli_epi16(t[3],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+0] =  _mm256_subs_epu16(t[0],t[8]);
        p256a[i+4] =  _mm256_subs_epu16(t[1],t[9]);
        p256a[i+8] =  _mm256_subs_epu16(t[2],t[10]);
        p256a[i+12] =  _mm256_subs_epu16(t[3],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[4],14);
        t[9] =  _mm256_srli_epi16(t[5],14);
        t[10] = _mm256_srli_epi16(t[6],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+16] =  _mm256_subs_epu16(t[4],t[8]);
        p256a[i+20] =  _mm256_subs_epu16(t[5],t[9]);
        p256a[i+24] =  _mm256_subs_epu16(t[6],t[10]);
        p256a[i+28] =  _mm256_subs_epu16(t[7],t[11]);
     }   
    for(i=0; i< 2; i++)
    {
    //first round
        //level 7
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+8],p256zeta[1]);
        t[4] = _mm256_mulhi_epu16(p256a[i+8],p256zeta[1]);
        t[9] = _mm256_mullo_epi16(p256a[i+10],p256zeta[1]);
        t[5] = _mm256_mulhi_epu16(p256a[i+10],p256zeta[1]);
        t[10] = _mm256_mullo_epi16(p256a[i+12],p256zeta[1]);
        t[6] = _mm256_mulhi_epu16(p256a[i+12],p256zeta[1]);
        t[11] = _mm256_mullo_epi16(p256a[i+14],p256zeta[1]);
        t[7] = _mm256_mulhi_epu16(p256a[i+14],p256zeta[1]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);
        
        
        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[4] =  _mm256_adds_epu16(p256a[i],v2q16x);
        t[5] =  _mm256_adds_epu16(p256a[i+2],v2q16x);
        t[6] =  _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[7] =  _mm256_adds_epu16(p256a[i+6],v2q16x);
        
        t[4] =  _mm256_subs_epu16(t[4],t[8]);
        t[5] =  _mm256_subs_epu16(t[5],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
    
        t[0] =  _mm256_adds_epu16(p256a[i],t[8]);
        t[1] =  _mm256_adds_epu16(p256a[i+2],t[9]);
        t[2] =  _mm256_adds_epu16(p256a[i+4],t[10]);
        t[3] =  _mm256_adds_epu16(p256a[i+6],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
  
        //level 6
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[3]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[3]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[3]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[3]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[4]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[4]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[4]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[4]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[2] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[1],v2q16x);
        t[6] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[5],v2q16x);
        
        t[2] =  _mm256_subs_epu16(t[2],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[1] =  _mm256_adds_epu16(t[1],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[5] =  _mm256_adds_epu16(t[5],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
       
        //level 5
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[7]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[7]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[8]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[8]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[9]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[9]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[10]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[10]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
  
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        // reduce to [0,2*Q)
        
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+2] =  _mm256_subs_epu16(t[1],t[8]);
        p256a[i+6] =  _mm256_subs_epu16(t[3],t[9]);
        p256a[i+10] =  _mm256_subs_epu16(t[5],t[10]);
        p256a[i+14] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+0]=  _mm256_subs_epu16(t[0],t[8]);
        p256a[i+4] =  _mm256_subs_epu16(t[2],t[9]);
        p256a[i+8] =  _mm256_subs_epu16(t[4],t[10]);
        p256a[i+12] =  _mm256_subs_epu16(t[6],t[11]);

        //second round
         //level 7
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+24],p256zeta[2]);
        t[4] = _mm256_mulhi_epu16(p256a[i+24],p256zeta[2]);
        t[9] = _mm256_mullo_epi16(p256a[i+26],p256zeta[2]);
        t[5] = _mm256_mulhi_epu16(p256a[i+26],p256zeta[2]);
        t[10] = _mm256_mullo_epi16(p256a[i+28],p256zeta[2]);
        t[6] = _mm256_mulhi_epu16(p256a[i+28],p256zeta[2]);
        t[11] = _mm256_mullo_epi16(p256a[i+30],p256zeta[2]);
        t[7] = _mm256_mulhi_epu16(p256a[i+30],p256zeta[2]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);
        
        
        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[4] =  _mm256_adds_epu16(p256a[i+16],v2q16x);
        t[5] =  _mm256_adds_epu16(p256a[i+18],v2q16x);
        t[6] =  _mm256_adds_epu16(p256a[i+20],v2q16x);
        t[7] =  _mm256_adds_epu16(p256a[i+22],v2q16x);
        
        t[4] =  _mm256_subs_epu16(t[4],t[8]);
        t[5] =  _mm256_subs_epu16(t[5],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
    
        t[0] =  _mm256_adds_epu16(p256a[i+16],t[8]);
        t[1] =  _mm256_adds_epu16(p256a[i+18],t[9]);
        t[2] =  _mm256_adds_epu16(p256a[i+20],t[10]);
        t[3] =  _mm256_adds_epu16(p256a[i+22],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
            
        //level 6
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[5]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[5]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[5]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[5]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[6]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[6]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[6]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[6]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly
        t[2] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[1],v2q16x);
        t[6] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[5],v2q16x);
        
        t[2] =  _mm256_subs_epu16(t[2],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[6] =  _mm256_subs_epu16(t[6],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[1] =  _mm256_adds_epu16(t[1],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[5] =  _mm256_adds_epu16(t[5],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
       
        //level 5
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[11]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[11]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[12]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[12]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[13]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[13]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[14]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[14]);
          
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
  
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[18+i] =  _mm256_subs_epu16(t[1],t[8]);
        p256a[22+i] =  _mm256_subs_epu16(t[3],t[9]);
        p256a[26+i] =  _mm256_subs_epu16(t[5],t[10]);
        p256a[30+i] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[16+i]=  _mm256_subs_epu16(t[0],t[8]);
        p256a[20+i] =  _mm256_subs_epu16(t[2],t[9]);
        p256a[24+i] =  _mm256_subs_epu16(t[4],t[10]);
        p256a[28+i] =  _mm256_subs_epu16(t[6],t[11]);
    }
    j=0;
    for(i=0; i< 32; i+= 8,j+=4)
    {
        //level 4
        //mul
        t[8] = _mm256_mullo_epi16(p256a[i+1],p256zeta[15+j]);
        t[1] = _mm256_mulhi_epu16(p256a[i+1],p256zeta[15+j]);
        t[9] = _mm256_mullo_epi16(p256a[i+3],p256zeta[16+j]);
        t[3] = _mm256_mulhi_epu16(p256a[i+3],p256zeta[16+j]);
        t[10] = _mm256_mullo_epi16(p256a[i+5],p256zeta[17+j]);
        t[5] = _mm256_mulhi_epu16(p256a[i+5],p256zeta[17+j]);
        t[11] = _mm256_mullo_epi16(p256a[i+7],p256zeta[18+j]);
        t[7] = _mm256_mulhi_epu16(p256a[i+7],p256zeta[18+j]);
        
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);


        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(p256a[i+0],v2q16x);
        t[3] = _mm256_adds_epu16(p256a[i+2],v2q16x);
        t[5] = _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[7] = _mm256_adds_epu16(p256a[i+6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
           
        t[0] =  _mm256_adds_epu16(p256a[i+0],t[8]);
        t[2] =  _mm256_adds_epu16(p256a[i+2],t[9]);
        t[4] =  _mm256_adds_epu16(p256a[i+4],t[10]);
        t[6] =  _mm256_adds_epu16(p256a[i+6],t[11]);       
        
        //reduce to [0,2Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[8]=  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);

        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 

        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);


        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[8]=  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);
 
        //level 3  
        t[0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        t[1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        t[2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        t[3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        t[4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        t[5] = _mm256_permute2x128_si256(t[10],t[5],0x31);
        t[6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        t[7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
        
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[31+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[31+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[32+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[32+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[33+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[33+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[34+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[34+j]);
         

        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);

        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        
        //reduce to [0,2Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[8]=  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);
                           
         //level 2  
        t[8] = _mm256_permute4x64_epi64(t[8],0xb1);
        t[9] = _mm256_permute4x64_epi64(t[9],0xb1);
        t[10] = _mm256_permute4x64_epi64(t[10],0xb1);
        t[11] = _mm256_permute4x64_epi64(t[11],0xb1);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xcc);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xcc);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xcc);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xcc);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xcc);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xcc);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xcc);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xcc);
  
        t[0] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[2] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[4] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[6] = _mm256_permute4x64_epi64(t[6],0xb1);
        
        
         //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[47+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[47+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[48+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[48+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[49+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[49+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[50+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[50+j]);
         

        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);

        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        //reduce to [0,2Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 1         
        t[8] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[9] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[10] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[11] = _mm256_permutevar8x32_epi32(t[6],idx);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xaa);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xaa);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xaa);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xaa);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xaa);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xaa);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xaa);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xaa);
  
        t[0] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[2] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[4] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[6] = _mm256_permutevar8x32_epi32(t[6],idx);
        
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[63+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[63+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[64+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[64+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[65+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[65+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[66+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[66+j]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
        
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        //reduce to [0,2Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 0
        t[8] = _mm256_srli_epi32(t[0],16);
        t[9] = _mm256_srli_epi32(t[2],16); 
        t[10] = _mm256_slli_epi32(t[1],16);
        t[11] = _mm256_slli_epi32(t[3],16);
        
        t[0] = _mm256_blend_epi16(t[0],t[10],0xaa);
        t[1] = _mm256_blend_epi16(t[8],t[1],0xaa);
        t[2] = _mm256_blend_epi16(t[2],t[11],0xaa);
        t[3] = _mm256_blend_epi16(t[9],t[3],0xaa);
         
        t[8] = _mm256_srli_epi32(t[4],16);
        t[9] = _mm256_srli_epi32(t[6],16); 
        t[10] = _mm256_slli_epi32(t[5],16);
        t[11] = _mm256_slli_epi32(t[7],16);
        
        t[4] = _mm256_blend_epi16(t[4],t[10],0xaa);
        t[5] = _mm256_blend_epi16(t[8],t[5],0xaa);
        t[6] = _mm256_blend_epi16(t[6],t[11],0xaa);
        t[7] = _mm256_blend_epi16(t[9],t[7],0xaa);
        
     
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[79+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[79+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[80+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[80+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[81+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[81+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[82+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[82+j]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[8] = _mm256_add_epi16(t[8],vq16x);
        t[9] = _mm256_add_epi16(t[9],vq16x);
        t[10] = _mm256_add_epi16(t[10],vq16x);
        t[11] = _mm256_add_epi16(t[11],vq16x);
        
        //butterfly      
        t[1] = _mm256_adds_epu16(t[0],v2q16x);
        t[3] = _mm256_adds_epu16(t[2],v2q16x);
        t[5] = _mm256_adds_epu16(t[4],v2q16x);
        t[7] = _mm256_adds_epu16(t[6],v2q16x);
        
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);
      
        t[0] =  _mm256_adds_epu16(t[0],t[8]);
        t[2] =  _mm256_adds_epu16(t[2],t[9]);
        t[4] =  _mm256_adds_epu16(t[4],t[10]);
        t[6] =  _mm256_adds_epu16(t[6],t[11]);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[1],14);
        t[9] =  _mm256_srli_epi16(t[3],14);
        t[10] = _mm256_srli_epi16(t[5],14);
        t[11] = _mm256_srli_epi16(t[7],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[1] =  _mm256_subs_epu16(t[1],t[8]);
        t[3] =  _mm256_subs_epu16(t[3],t[9]);
        t[5] =  _mm256_subs_epu16(t[5],t[10]);
        t[7] =  _mm256_subs_epu16(t[7],t[11]);

        
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
            
        //store   
        t[8] = _mm256_unpacklo_epi16(t[0],t[1]);
        t[1] = _mm256_unpackhi_epi16(t[0],t[1]);
        t[9] = _mm256_unpacklo_epi16(t[2],t[3]);
        t[3] = _mm256_unpackhi_epi16(t[2],t[3]);
        t[10] = _mm256_unpacklo_epi16(t[4],t[5]);
        t[5] = _mm256_unpackhi_epi16(t[4],t[5]);
        t[11] = _mm256_unpacklo_epi16(t[6],t[7]);
        t[7] = _mm256_unpackhi_epi16(t[6],t[7]);
        
        p256a[i+0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        p256a[i+1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        p256a[i+2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        p256a[i+3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        p256a[i+4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        p256a[i+5] = _mm256_permute2x128_si256(t[10],t[5],0x31); 
        p256a[i+6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        p256a[i+7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
    }
}

void invntt(uint16_t a[PARAM_N])
{
    int i,j;
    __m256i *p256a = (__m256i *) a;
    __m256i *p256zeta = (__m256i *) zetas_inv_avx;
     
    __m256i vq16x = _mm256_set1_epi16(PARAM_Q);
    __m256i v2q16x =  _mm256_set1_epi16(2*PARAM_Q);
    __m256i vqinv16x = _mm256_set1_epi16(53249U); //inverse_mod(q,2^16)
    __m256i invn16x = _mm256_set1_epi16(128); //invn*2^16 mod q

    __m256i t[16];
    __m256i idx = _mm256_set_epi32(6,7,4,5,2,3,0,1);
    __m256i mask = _mm256_set1_epi32(0xFFFF);
    
    
    j=0;
    for(i=0;i<32;i+=8,j+=4)
    {
        //level 0    
        //pack the data
        t[8] = _mm256_permute2x128_si256(p256a[i+0],p256a[i+1],0x20);
        t[1] = _mm256_permute2x128_si256(p256a[i+0],p256a[i+1],0x31);
        t[9] = _mm256_permute2x128_si256(p256a[i+2],p256a[i+3],0x20);
        t[3] = _mm256_permute2x128_si256(p256a[i+2],p256a[i+3],0x31);
        t[10]= _mm256_permute2x128_si256(p256a[i+4],p256a[i+5],0x20);
        t[5] = _mm256_permute2x128_si256(p256a[i+4],p256a[i+5],0x31); 
        t[11]= _mm256_permute2x128_si256(p256a[i+6],p256a[i+7],0x20);
        t[7] = _mm256_permute2x128_si256(p256a[i+6],p256a[i+7],0x31);

        t[12] = _mm256_and_si256(t[8],mask);
        t[13] = _mm256_and_si256(t[9],mask);
        t[14] = _mm256_and_si256(t[1],mask);
        t[15] = _mm256_and_si256(t[3],mask);
        t[0] = _mm256_packus_epi32(t[12],t[14]);
        t[2] = _mm256_packus_epi32(t[13],t[15]);
        
        t[12] = _mm256_and_si256(t[10],mask);
        t[13] = _mm256_and_si256(t[11],mask);
        t[14] = _mm256_and_si256(t[5],mask);
        t[15] = _mm256_and_si256(t[7],mask);
        t[4] = _mm256_packus_epi32(t[12],t[14]);
        t[6] = _mm256_packus_epi32(t[13],t[15]);
        
        
        t[8] = _mm256_srli_epi32(t[8],16);
        t[1] = _mm256_srli_epi32(t[1],16);
        t[9] = _mm256_srli_epi32(t[9],16);
        t[3] = _mm256_srli_epi32(t[3],16);
        t[10]= _mm256_srli_epi32(t[10],16);
        t[5] = _mm256_srli_epi32(t[5],16);
        t[11]= _mm256_srli_epi32(t[11],16);
        t[7] = _mm256_srli_epi32(t[7],16);

        t[1] = _mm256_packus_epi32(t[8],t[1]);
        t[3] = _mm256_packus_epi32(t[9],t[3]);
        t[5] = _mm256_packus_epi32(t[10],t[5]);
        t[7] = _mm256_packus_epi32(t[11],t[7]);
          
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[2],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[6],v2q16x);
        
        t[0] = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[j+0]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[j+0]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[j+1]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[j+1]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[j+2]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[j+2]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[j+3]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[j+3]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
       
        //level 1   
        t[8] = _mm256_srli_epi32(t[0],16);
        t[9] = _mm256_srli_epi32(t[2],16); 
        t[10] = _mm256_slli_epi32(t[1],16);
        t[11] = _mm256_slli_epi32(t[3],16);
        
        t[0] = _mm256_blend_epi16(t[0],t[10],0xaa);
        t[1] = _mm256_blend_epi16(t[8],t[1],0xaa);
        t[2] = _mm256_blend_epi16(t[2],t[11],0xaa);
        t[3] = _mm256_blend_epi16(t[9],t[3],0xaa);
         
        t[8] = _mm256_srli_epi32(t[4],16);
        t[9] = _mm256_srli_epi32(t[6],16); 
        t[10] = _mm256_slli_epi32(t[5],16);
        t[11] = _mm256_slli_epi32(t[7],16);
        
        t[4] = _mm256_blend_epi16(t[4],t[10],0xaa);
        t[5] = _mm256_blend_epi16(t[8],t[5],0xaa);
        t[6] = _mm256_blend_epi16(t[6],t[11],0xaa);
        t[7] = _mm256_blend_epi16(t[9],t[7],0xaa);
        
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[2],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[6],v2q16x);
        
        t[0] = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[16+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[16+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[17+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[17+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[18+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[18+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[19+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[19+j]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 2
        t[8] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[9] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[10] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[11] = _mm256_permutevar8x32_epi32(t[6],idx);
            
        t[0] = _mm256_blend_epi32(t[1],t[8],0xaa);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xaa);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xaa);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xaa);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xaa);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xaa);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xaa);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xaa);
  
        t[0] = _mm256_permutevar8x32_epi32(t[0],idx);
        t[2] = _mm256_permutevar8x32_epi32(t[2],idx);
        t[4] = _mm256_permutevar8x32_epi32(t[4],idx);
        t[6] = _mm256_permutevar8x32_epi32(t[6],idx);
        
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[2],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[6],v2q16x);
        
        t[0] = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[32+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[32+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[33+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[33+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[34+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[34+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[35+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[35+j]);    
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 3 
        t[8] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[9] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[10] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[11] = _mm256_permute4x64_epi64(t[6],0xb1);
           
        t[0] = _mm256_blend_epi32(t[1],t[8],0xcc);
        t[1] = _mm256_blend_epi32(t[8],t[1],0xcc);      
        t[2] = _mm256_blend_epi32(t[3],t[9],0xcc);
        t[3] = _mm256_blend_epi32(t[9],t[3],0xcc);      
        t[4] = _mm256_blend_epi32(t[5],t[10],0xcc);
        t[5] = _mm256_blend_epi32(t[10],t[5],0xcc);  
        t[6] = _mm256_blend_epi32(t[7],t[11],0xcc);
        t[7] = _mm256_blend_epi32(t[11],t[7],0xcc);
  
        t[0] = _mm256_permute4x64_epi64(t[0],0xb1);
        t[2] = _mm256_permute4x64_epi64(t[2],0xb1);
        t[4] = _mm256_permute4x64_epi64(t[4],0xb1);
        t[6] = _mm256_permute4x64_epi64(t[6],0xb1);
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[2],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[6],v2q16x);
        
        t[0] = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[48+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[48+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[49+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[49+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[50+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[50+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[51+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[51+j]);    
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[8]=  _mm256_subs_epu16(t[0],t[8]);
        t[9] =  _mm256_subs_epu16(t[2],t[9]);
        t[10] =  _mm256_subs_epu16(t[4],t[10]);
        t[11] =  _mm256_subs_epu16(t[6],t[11]);
        
        //level 4   
        t[0] = _mm256_permute2x128_si256(t[8],t[1],0x20);
        t[1] = _mm256_permute2x128_si256(t[8],t[1],0x31);
        t[2] = _mm256_permute2x128_si256(t[9],t[3],0x20);
        t[3] = _mm256_permute2x128_si256(t[9],t[3],0x31);
        t[4] = _mm256_permute2x128_si256(t[10],t[5],0x20);
        t[5] = _mm256_permute2x128_si256(t[10],t[5],0x31);
        t[6] = _mm256_permute2x128_si256(t[11],t[7],0x20);
        t[7] = _mm256_permute2x128_si256(t[11],t[7],0x31);
        
        //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[2],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[6],v2q16x);
        
        t[0] = _mm256_adds_epu16(t[0],t[1]);
        t[2]  = _mm256_adds_epu16(t[2],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[5]);
        t[6]  = _mm256_adds_epu16(t[6],t[7]);
        
        t[1]  = _mm256_subs_epu16(t[8],t[1]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[5]  = _mm256_subs_epu16(t[10],t[5]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[64+j]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[64+j]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[65+j]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[65+j]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[66+j]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[66+j]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[67+j]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[67+j]);      
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[1] = _mm256_sub_epi16(t[1],t[8]);
        t[3] = _mm256_sub_epi16(t[3],t[9]);
        t[5] = _mm256_sub_epi16(t[5],t[10]);
        t[7] = _mm256_sub_epi16(t[7],t[11]);
        
        p256a[i+1]  = _mm256_add_epi16(t[1],vq16x);
        p256a[i+3]  = _mm256_add_epi16(t[3],vq16x);
        p256a[i+5]  = _mm256_add_epi16(t[5],vq16x);
        p256a[i+7]  = _mm256_add_epi16(t[7],vq16x);
        
        // reduce to [0,2*Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+0] =  _mm256_subs_epu16(t[0],t[8]);
        p256a[i+2] =  _mm256_subs_epu16(t[2],t[9]);
        p256a[i+4] =  _mm256_subs_epu16(t[4],t[10]);
        p256a[i+6] =  _mm256_subs_epu16(t[6],t[11]);
    }
     
    for(i=0;i<2;i++)
    {
    	   //first round
    	  //level 5  
       //butterfly 
        t[8]  = _mm256_adds_epu16(p256a[i+0],v2q16x);
        t[9]  = _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[10]  = _mm256_adds_epu16(p256a[i+8],v2q16x);
        t[11]  = _mm256_adds_epu16(p256a[i+12],v2q16x);
        
        
        t[0]  = _mm256_adds_epu16(p256a[i+0],p256a[i+2]);
        t[2]  = _mm256_adds_epu16(p256a[i+4],p256a[i+6]);
        t[4]  = _mm256_adds_epu16(p256a[i+8],p256a[i+10]);
        t[6]  = _mm256_adds_epu16(p256a[i+12],p256a[i+14]);
           
        t[1]  = _mm256_subs_epu16(t[8],p256a[i+2]);
        t[3]  = _mm256_subs_epu16(t[9],p256a[i+6]);
        t[5]  = _mm256_subs_epu16(t[10],p256a[i+10]);
        t[7]  = _mm256_subs_epu16(t[11],p256a[i+14]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[80]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[80]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[81]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[81]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[82]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[82]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[83]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[83]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
         
    	  //level 6
    	   //butterfly     
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[1],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[5],v2q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[2]);
        t[1]  = _mm256_adds_epu16(t[1],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[6]);
        t[5]  = _mm256_adds_epu16(t[5],t[7]);
           
        t[2]  = _mm256_subs_epu16(t[8],t[2]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[88]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[88]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[88]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[88]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[89]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[89]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[89]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[89]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[2] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[6] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
    	  
    	  //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[1],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[5],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[1] =  _mm256_subs_epu16(t[1],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[5] =  _mm256_subs_epu16(t[5],t[11]);
        
    	  //level 7
    	  //butterfly    
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[1],v2q16x);
        t[10]  = _mm256_adds_epu16(t[2],v2q16x);
        t[11]  = _mm256_adds_epu16(t[3],v2q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[4]);
        t[1]  = _mm256_adds_epu16(t[1],t[5]);
        t[2]  = _mm256_adds_epu16(t[2],t[6]);
        t[3]  = _mm256_adds_epu16(t[3],t[7]);
           
        t[4]  = _mm256_subs_epu16(t[8],t[4]);
        t[5]  = _mm256_subs_epu16(t[9],t[5]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[4],p256zeta[92]);
        t[4] = _mm256_mulhi_epu16(t[4],p256zeta[92]);
        t[9] = _mm256_mullo_epi16(t[5],p256zeta[92]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[92]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[92]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[92]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[92]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[92]);
        
        //reduce 
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        p256a[i+8] = _mm256_add_epi16(t[8],vq16x);
        p256a[i+10] = _mm256_add_epi16(t[9],vq16x);
        p256a[i+12] = _mm256_add_epi16(t[10],vq16x);
        p256a[i+14] = _mm256_add_epi16(t[11],vq16x);
         
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[1],14);
        t[10] = _mm256_srli_epi16(t[2],14);
        t[11] = _mm256_srli_epi16(t[3],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+0] =  _mm256_subs_epu16(t[0],t[8]);
        p256a[i+2] =  _mm256_subs_epu16(t[1],t[9]);
        p256a[i+4] =  _mm256_subs_epu16(t[2],t[10]);
        p256a[i+6] =  _mm256_subs_epu16(t[3],t[11]);

        //second round
        //level 5  
       //butterfly 
        t[8]  = _mm256_adds_epu16(p256a[i+16],v2q16x);
        t[9]  = _mm256_adds_epu16(p256a[i+20],v2q16x);
        t[10]  = _mm256_adds_epu16(p256a[i+24],v2q16x);
        t[11]  = _mm256_adds_epu16(p256a[i+28],v2q16x);
        
        
        t[0]  = _mm256_adds_epu16(p256a[i+16],p256a[i+18]);
        t[2]  = _mm256_adds_epu16(p256a[i+20],p256a[i+22]);
        t[4]  = _mm256_adds_epu16(p256a[i+24],p256a[i+26]);
        t[6]  = _mm256_adds_epu16(p256a[i+28],p256a[i+30]);
           
        t[1]  = _mm256_subs_epu16(t[8],p256a[i+18]);
        t[3]  = _mm256_subs_epu16(t[9],p256a[i+22]);
        t[5]  = _mm256_subs_epu16(t[10],p256a[i+26]);
        t[7]  = _mm256_subs_epu16(t[11],p256a[i+30]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[1],p256zeta[84]);
        t[1] = _mm256_mulhi_epu16(t[1],p256zeta[84]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[85]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[85]);
        t[10] = _mm256_mullo_epi16(t[5],p256zeta[86]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[86]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[87]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[87]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[1],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[5],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[1] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[5] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[2],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[6],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[2] =  _mm256_subs_epu16(t[2],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[6] =  _mm256_subs_epu16(t[6],t[11]);
         
    	  //level 6
    	   //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[1],v2q16x);
        t[10]  = _mm256_adds_epu16(t[4],v2q16x);
        t[11]  = _mm256_adds_epu16(t[5],v2q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[2]);
        t[1]  = _mm256_adds_epu16(t[1],t[3]);
        t[4]  = _mm256_adds_epu16(t[4],t[6]);
        t[5]  = _mm256_adds_epu16(t[5],t[7]);
           
        t[2]  = _mm256_subs_epu16(t[8],t[2]);
        t[3]  = _mm256_subs_epu16(t[9],t[3]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[2],p256zeta[90]);
        t[2] = _mm256_mulhi_epu16(t[2],p256zeta[90]);
        t[9] = _mm256_mullo_epi16(t[3],p256zeta[90]);
        t[3] = _mm256_mulhi_epu16(t[3],p256zeta[90]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[91]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[91]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[91]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[91]);   
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[2],t[8]);
        t[9] = _mm256_sub_epi16(t[3],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        t[2] = _mm256_add_epi16(t[8],vq16x);
        t[3] = _mm256_add_epi16(t[9],vq16x);
        t[6] = _mm256_add_epi16(t[10],vq16x);
        t[7] = _mm256_add_epi16(t[11],vq16x);
    	  
    	  
    	  //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[1],14);
        t[10] = _mm256_srli_epi16(t[4],14);
        t[11] = _mm256_srli_epi16(t[5],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        t[0]=  _mm256_subs_epu16(t[0],t[8]);
        t[1] =  _mm256_subs_epu16(t[1],t[9]);
        t[4] =  _mm256_subs_epu16(t[4],t[10]);
        t[5] =  _mm256_subs_epu16(t[5],t[11]);
        
    	  //level 7
    	  //butterfly 
        t[8]  = _mm256_adds_epu16(t[0],v2q16x);
        t[9]  = _mm256_adds_epu16(t[1],v2q16x);
        t[10]  = _mm256_adds_epu16(t[2],v2q16x);
        t[11]  = _mm256_adds_epu16(t[3],v2q16x);
          
        t[0]  = _mm256_adds_epu16(t[0],t[4]);
        t[1]  = _mm256_adds_epu16(t[1],t[5]);
        t[2]  = _mm256_adds_epu16(t[2],t[6]);
        t[3]  = _mm256_adds_epu16(t[3],t[7]);
           
        t[4]  = _mm256_subs_epu16(t[8],t[4]);
        t[5]  = _mm256_subs_epu16(t[9],t[5]);
        t[6]  = _mm256_subs_epu16(t[10],t[6]);
        t[7]  = _mm256_subs_epu16(t[11],t[7]);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[4],p256zeta[93]);
        t[4] = _mm256_mulhi_epu16(t[4],p256zeta[93]);
        t[9] = _mm256_mullo_epi16(t[5],p256zeta[93]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[93]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[93]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[93]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[93]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[93]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
        
        p256a[i+24] = _mm256_add_epi16(t[8],vq16x);
        p256a[i+26] = _mm256_add_epi16(t[9],vq16x);
        p256a[i+28] = _mm256_add_epi16(t[10],vq16x);
        p256a[i+30] = _mm256_add_epi16(t[11],vq16x);
        
        //reduce to [0,2*PARAM_Q)
        t[8] =  _mm256_srli_epi16(t[0],14);
        t[9] =  _mm256_srli_epi16(t[1],14);
        t[10] = _mm256_srli_epi16(t[2],14);
        t[11] = _mm256_srli_epi16(t[3],14);
        
        t[8] =  _mm256_mullo_epi16(t[8],vq16x);
        t[9] =  _mm256_mullo_epi16(t[9],vq16x);
        t[10] = _mm256_mullo_epi16(t[10],vq16x);
        t[11] = _mm256_mullo_epi16(t[11],vq16x);
 
        p256a[i+16] =  _mm256_subs_epu16(t[0],t[8]);
        p256a[i+18] =  _mm256_subs_epu16(t[1],t[9]);
        p256a[i+20] =  _mm256_subs_epu16(t[2],t[10]);
        p256a[i+22] =  _mm256_subs_epu16(t[3],t[11]);  
    }
    for(i=0; i< 4; i++)
    {
        //level 8
        //butterfly
        t[4] =  _mm256_adds_epu16(p256a[i],v2q16x);
        t[5] =  _mm256_adds_epu16(p256a[i+4],v2q16x);
        t[6] =  _mm256_adds_epu16(p256a[i+8],v2q16x);
        t[7] =  _mm256_adds_epu16(p256a[i+12],v2q16x);
        
        t[0] =  _mm256_adds_epu16(p256a[i],p256a[i+16]);
        t[1] =  _mm256_adds_epu16(p256a[i+4],p256a[i+20]);
        t[2] =  _mm256_adds_epu16(p256a[i+8],p256a[i+24]);
        t[3] =  _mm256_adds_epu16(p256a[i+12],p256a[i+28]);
        
        t[4] =  _mm256_subs_epu16(t[4],p256a[i+16]);
        t[5] =  _mm256_subs_epu16(t[5],p256a[i+20]);
        t[6] =  _mm256_subs_epu16(t[6],p256a[i+24]);
        t[7] =  _mm256_subs_epu16(t[7],p256a[i+28]);
            
        //mul invn
        t[8] = _mm256_mullo_epi16(t[0],invn16x);
        t[0] = _mm256_mulhi_epu16(t[0],invn16x);
        t[9] = _mm256_mullo_epi16(t[1],invn16x);
        t[1] = _mm256_mulhi_epu16(t[1],invn16x);
        t[10] = _mm256_mullo_epi16(t[2],invn16x);
        t[2] = _mm256_mulhi_epu16(t[2],invn16x);
        t[11] = _mm256_mullo_epi16(t[3],invn16x);
        t[3] = _mm256_mulhi_epu16(t[3],invn16x);
      
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);

        t[8] = _mm256_sub_epi16(t[0],t[8]);
        t[9] = _mm256_sub_epi16(t[1],t[9]);
        t[10] = _mm256_sub_epi16(t[2],t[10]);
        t[11] = _mm256_sub_epi16(t[3],t[11]);
        
        p256a[i+0] = _mm256_add_epi16(t[8],vq16x);
        p256a[i+4] = _mm256_add_epi16(t[9],vq16x);
        p256a[i+8] = _mm256_add_epi16(t[10],vq16x);
        p256a[i+12] = _mm256_add_epi16(t[11],vq16x);
        
        //mul
        t[8] = _mm256_mullo_epi16(t[4],p256zeta[94]);
        t[4] = _mm256_mulhi_epu16(t[4],p256zeta[94]);
        t[9] = _mm256_mullo_epi16(t[5],p256zeta[94]);
        t[5] = _mm256_mulhi_epu16(t[5],p256zeta[94]);
        t[10] = _mm256_mullo_epi16(t[6],p256zeta[94]);
        t[6] = _mm256_mulhi_epu16(t[6],p256zeta[94]);
        t[11] = _mm256_mullo_epi16(t[7],p256zeta[94]);
        t[7] = _mm256_mulhi_epu16(t[7],p256zeta[94]);
        
        //reduce
        t[8] = _mm256_mullo_epi16(t[8],vqinv16x);
        t[9] = _mm256_mullo_epi16(t[9],vqinv16x);
        t[10] = _mm256_mullo_epi16(t[10],vqinv16x);
        t[11] = _mm256_mullo_epi16(t[11],vqinv16x);
        
        t[8] = _mm256_mulhi_epu16(t[8],vq16x);
        t[9] = _mm256_mulhi_epu16(t[9],vq16x);
        t[10] = _mm256_mulhi_epu16(t[10],vq16x);
        t[11] = _mm256_mulhi_epu16(t[11],vq16x);
        
        
        t[8] = _mm256_sub_epi16(t[4],t[8]);
        t[9] = _mm256_sub_epi16(t[5],t[9]);
        t[10] = _mm256_sub_epi16(t[6],t[10]);
        t[11] = _mm256_sub_epi16(t[7],t[11]);
   
        p256a[i+16] = _mm256_add_epi16(t[8],vq16x);
        p256a[i+20] = _mm256_add_epi16(t[9],vq16x);
        p256a[i+24] = _mm256_add_epi16(t[10],vq16x);
        p256a[i+28] = _mm256_add_epi16(t[11],vq16x);
     }
}
#endif

