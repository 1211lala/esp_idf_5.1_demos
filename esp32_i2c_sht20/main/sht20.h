#ifndef _SHT20_H_
#define _SHT20_H_

#include "common.h"
#include "driver/i2c.h"
#include "i2c.h"
void sht20_init(void);
uint8_t sht20_get_temp(float *temp);
uint8_t sht20_get_hum(float *hum);
unsigned char SHT20_CRC8(unsigned char *data, unsigned int datalen);
#endif