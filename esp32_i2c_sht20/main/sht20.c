#include "sht20.h"

#define SHT20_ADDR 0x40
#define SHT20_TEMP_CMD 0XF3
#define SHT20_HUM_CMD 0XF5

struct I2C_PARAM i2c_param = {
    .i2c_num = I2C_NUM_0,
    .freq = 100000,
    .scl = 15,
    .sda = 16,
    .init = i2c_master_init,
    .read = i2c_master_read_bytes,
    .write = i2c_master_write_bytes,

};

const char *TAG = "SHT20";
/******************************************************************************
 * 函数描述: SHT20初始化程序，初始化使用到的IIC
 * 参  数1: IIC总线频率
 *******************************************************************************/
void sht20_init(void)
{
    i2c_param.init(&i2c_param);
}
/******************************************************************************
 * 函数描述: 获取SHT20的温度信息
 * 参  数1: float指针，用于保存温度数据
 * 返  回 : ESP_OK 执行成功 ，其他错误
 *******************************************************************************/
uint8_t sht20_get_temp(float *temp)
{
    uint8_t buf[3];

    i2c_param.read(&i2c_param, SHT20_ADDR, SHT20_TEMP_CMD, buf, 3);
    /*温度计算公式*/
    *temp = (175.72 * ((buf[0] << 8) | buf[1]) / 65536 - 46.85);
    return ESP_OK;
}

/******************************************************************************
 * 函数描述: 获取SHT20的湿度信息
 * 参  数1: float指针，用于保存湿度数据
 * 返  回 : ESP_OK 执行成功 ，其他错误
 *******************************************************************************/
uint8_t sht20_get_hum(float *hum)
{
    uint8_t buf[3];

    i2c_param.read(&i2c_param, SHT20_ADDR, SHT20_HUM_CMD, buf, 3);

    *hum = 125.0 * ((buf[0] << 8) | buf[1]) / 65536 - 6.0;
    return ESP_OK;
}

/* SHT20的效验函数 */
unsigned char SHT20_CRC8(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00; /* CRC初始值 */
    unsigned char wCPoly = 0x31; /* 多项式值 */
    unsigned char wChar = 0;

    while (datalen--)
    {
        wChar = *(data++);
        wCRCin = wCRCin ^ wChar; /* 第一个数据异或上初始值 */
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80) /* 如果最高位为1，则左移一位再异或上多项式值 */
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else /* 如果最高位为0，则左移一位 */
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}