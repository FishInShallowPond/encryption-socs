#ifndef CBD_H
#define CBD_H

#include <stdint.h>
#include "poly.h"

void cbdx(poly *r, const unsigned char *buf, uint16_t eta);
void cbd_etas(poly *r, const unsigned char *buf);
void cbd_etae(poly *r, const unsigned char *buf);
#endif
