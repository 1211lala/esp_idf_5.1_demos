#ifndef _I2C_H_
#define _I2C_H_

#include "common.h"
#include "driver/i2c.h"

struct I2C_PARAM
{
    uint8_t i2c_num;
    uint16_t freq;
    uint8_t sda;
    uint8_t scl;
    void (*init)(struct I2C_PARAM *i2c_param);
    void (*write)(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t *data, uint16_t len);
    void (*read)(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
};

void i2c_master_init(struct I2C_PARAM *i2c_param);
void i2c_master_write_bytes(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t *data, uint16_t len);
void i2c_master_read_bytes(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
#endif
