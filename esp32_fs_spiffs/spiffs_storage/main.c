#include "datafile.h"
#include "spiffs.h"
#include "sd_spi_fat.h"

void fatfs_test(void *arg)
{
    // sd_init_mount_fat();
    // fatfs_scan("/");
    fs_mount();
    while (1)
    {
        vTaskDelay(50);
    }
}

void app_main()
{
    xTaskCreate(fatfs_test, "fatfs_test", 1024 * 10, NULL, 5, NULL);
    vTaskDelete(NULL);
}
