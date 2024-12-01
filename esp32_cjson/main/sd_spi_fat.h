#ifndef _SD_SPI_FAT_H
#define _SD_SPI_FAT_H

#include "datafile.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "dirent.h"

struct SDFAT
{
    spi_host_device_t spi_host;                 /* spi主机编号 */
    uint16_t freq;                              /* spi速度 单位 Khz */
    uint8_t miso;                               /* spi引脚 */
    uint8_t mosi;                               /* spi引脚 */
    uint8_t sck;                                /* spi引脚 */
    uint8_t cs;                                 /* spi引脚 */
    uint16_t max_transfer_sz;                   /* spi一次传输的最大数据量, 具体参考 spi_bus_config_t.max_transfer_sz 注释*/
    char *root_path;                            /* sdfat的根目录名称 */
    esp_vfs_fat_sdmmc_mount_config_t mount_cfg; /* 挂载参数，参考官方例程  fatfsgen 这个结构体参数实际使用可以删除,在初始化函数中使用就可以,后期释放资源并没有使用到它 */
    sdmmc_card_t *card;
};

extern struct SDFAT sdfat;

void sdfat_mount(struct SDFAT *fat);
int fatfs_write(const char *path, char *data, uint32_t size);
int fatfs_read(const char *path, char **buffer, uint32_t max_size);
int isexist(const char *path);
FRESULT fatfs_scan(char *path);
#endif