#ifndef _SPIFAT_H_
#define _SPIFAT_H_

#include "datafile.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_system.h"
#include "esp_vfs_fat.h"

#include "dirent.h"

struct FLASHFAT
{
    char *root_path;                         /* 设置标准C库下的flash介质下文件系统的根目录 */
    char *partition_label;                   /* 对应分区表的标签/名称 */
    esp_vfs_fat_mount_config_t mount_config; /* 挂载参数，参考官方例程  fatfsgen 这个结构体参数实际使用可以删除,在初始化函数中使用就可以,后期释放资源并没有使用到它*/
    wl_handle_t wl_handle;                   /* 磨损介质句柄,参考官方例程 fatfsgen */
};

void flashfat_mount(struct FLASHFAT *fat);
void list_files(const char *path);

#endif
