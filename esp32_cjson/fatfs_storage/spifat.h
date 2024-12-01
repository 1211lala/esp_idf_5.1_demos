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
    char *root_path;
    char *partition_label;
    esp_vfs_fat_mount_config_t mount_config;
    wl_handle_t wl_handle;
};

void flashfat_mount(struct FLASHFAT *fat);


#endif
