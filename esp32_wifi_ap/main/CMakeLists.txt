idf_component_register(SRCS  
                        "main.c" 
                        "wifi_ap.c"
                       INCLUDE_DIRS ".")

# 将 ../spiffs_data 的内容以 spiffs or fatfs or littlefs 的格式烧录进 storage 分区内

# spiffs_create_partition_image(storage ../spiffs_data FLASH_IN_PROJECT)
# fatfs_create_partition_image(storage ../spiffs_data FLASH_IN_PROJECT)
# littlefs_create_partition_image(storage ../spiffs_data FLASH_IN_PROJECT)CT)T)