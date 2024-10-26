#ifndef _SPIFFS_H_
#define _SPIFFS_H_


#include "common.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_system.h"
#include "esp_spiffs.h"
#include "dirent.h"

extern esp_vfs_spiffs_conf_t conf;

void spiffs_mount(void);
void spiffs_scan(void);
#endif

