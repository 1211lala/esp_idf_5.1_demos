#include "datafile.h"
#include "spifat.h"
#include "sd_spi_fat.h"

struct FLASHFAT flashfat = {

    .root_path = "/flashroot",
    .partition_label = "storage",
    .mount_config.allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
    .mount_config.disk_status_check_enable = false,
    .mount_config.format_if_mount_failed = false,
    .mount_config.max_files = 3,
    .wl_handle = WL_INVALID_HANDLE,
};

struct SDFAT sdfat = {
    .spi_host = SPI2_HOST,
    .freq = 20000,
    .cs = 4,
    .mosi = 23,
    .miso = 19,
    .sck = 18,
    .root_path = "/sdfat",
    .mount_cfg.allocation_unit_size = 1024 * 2,
    .mount_cfg.disk_status_check_enable = false,
    .mount_cfg.format_if_mount_failed = false,
    .mount_cfg.max_files = 4,
};
void fatfs_test(void *arg)
{
    flashfat_mount(&flashfat);
    sdfat_mount(&sdfat);
    list_files(sdfat.root_path);
    while (1)
    {
        vTaskDelay(50);
    }
}

void app_main()
{
    xTaskCreate(fatfs_test, "fatfs_test", 1024 * 5, NULL, 5, NULL);
    vTaskDelete(NULL);
}
