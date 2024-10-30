#include "i2c.h"

/******************************************************************************
 * 函数描述: SHT20初始化程序，初始化使用到的IIC
 *******************************************************************************/
void i2c_master_init(struct I2C_PARAM *i2c_param)
{
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = i2c_param->sda;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = i2c_param->scl;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = i2c_param->freq;

    i2c_param_config(i2c_param->i2c_num, &config);

    i2c_driver_install(i2c_param->i2c_num, I2C_MODE_MASTER, 0, 0, 0);
}
/******************************************************************************
 * 函数描述: addr为i2c 7位地址
 *******************************************************************************/
void i2c_master_write_bytes(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t *data, uint16_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    for (uint16_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            i2c_master_write_byte(cmd, data[i], false);
        }
        else
        {
            i2c_master_write_byte(cmd, data[i], true);
        }
    }
    i2c_master_stop(cmd);
    ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_param->i2c_num, cmd, 1000 / portTICK));
    i2c_cmd_link_delete(cmd);
}
/******************************************************************************
 * 函数描述: addr为i2c 7位地址
 *******************************************************************************/
void i2c_master_read_bytes(struct I2C_PARAM *i2c_param, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    i2c_master_write_bytes(i2c_param, addr, &reg, 1);

    vTaskDelay(100 / portTICK);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    for (uint16_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {

            i2c_master_read_byte(cmd, &data[i], I2C_MASTER_NACK);
        }
        else
        {
            i2c_master_read_byte(cmd, &data[i], I2C_MASTER_ACK);
        }
    }
    i2c_master_stop(cmd);
    ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_param->i2c_num, cmd, 100 / portTICK));
    i2c_cmd_link_delete(cmd);
}