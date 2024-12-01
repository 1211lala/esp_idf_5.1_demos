#include "datafile.h"
#include "spifat.h"
#include "sd_spi_fat.h"

struct FLASHFAT flashfat = {

    .root_path = "/spiflash",
    .partition_label = "storage",
    .mount_config.allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
    .mount_config.disk_status_check_enable = false,
    .mount_config.format_if_mount_failed = true,
    .mount_config.max_files = 5,
    .wl_handle = WL_INVALID_HANDLE,
};

void flashfat_test(void *arg)
{
    flashfat_mount(&flashfat);

    while (1)
    {
        vTaskDelay(50);
    }
}

void app_main()
{
    xTaskCreate(flashfat_test, "flashfat_test", 1024 * 10, NULL, 5, NULL);
    vTaskDelete(NULL);
}
