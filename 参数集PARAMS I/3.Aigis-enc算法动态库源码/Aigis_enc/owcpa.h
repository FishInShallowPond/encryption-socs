#ifndef OWCPA_H
#define OWCPA_H

void owcpa_keypair(unsigned char *pk, 
                   unsigned char *sk);

void owcpa_enc(unsigned char *c,
               const unsigned char *m,
               const unsigned char *pk,
               const unsigned char *coins);

void owcpa_dec(unsigned char *m,
               const unsigned char *c,
               const unsigned char *sk);

#endif
