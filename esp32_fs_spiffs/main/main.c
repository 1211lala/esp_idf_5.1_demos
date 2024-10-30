#include "datafile.h"
#include "spiffs.h"


void fatfs_test(void *arg)
{
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
