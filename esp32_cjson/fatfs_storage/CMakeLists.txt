idf_component_register(SRCS  
                        "main.c" 
                        "spifat.c"
                        "sd_spi_fat.c"
                       INCLUDE_DIRS ".")

# 将 ../spiffs_data 的内容以  fatfs  的格式烧录进 storage 分区内
# fatfs_create_spiflash_image(storage ../fatfs_storage FLASH_IN_PROJECT)