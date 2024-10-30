/* https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32/api-reference/storage/fatfs.html */

/**
    https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/storage/fatfs.html
    ESP-IDF 使用 FatFs 库来实现 FAT 文件系统
    如果使用了标志C库的文件函数 需要加上挂载文件系统的路径前缀 /sdcard/hello.c
    如果使用的 fatfs 的库函数则不用 /hello.c
    以下程序统一使用 fatfs 的函数

    使用长文件名打开 menuconfig 将 Long filename support 打开

    POSIX	FatFs
    "r"	    FA_READ
    "r+"	FA_READ | FA_WRITE
    "w"	    FA_CREATE_ALWAYS | FA_WRITE
    "w+"	FA_CREATE_ALWAYS | FA_WRITE | FA_READ
    "a"	    FA_OPEN_APPEND | FA_WRITE
    "a+"	FA_OPEN_APPEND | FA_WRITE | FA_READ
    "wx"	FA_CREATE_NEW | FA_WRITE
    "w+x"	FA_CREATE_NEW | FA_WRITE | FA_READ


    在 SPI flash 中初始化 FAT 文件系统并在 VFS 中注册的便捷功能。
    这是一个多合一功能，可执行以下操作：
    查找具有已定义partition_label的分区。分区标签应在分区表中配置。
    在给定分区之上初始化 Flash 磨损均衡库
    使用 FATFS 库将 FAT 分区挂载到 Flash 磨损均衡库之上
    使用 VFS 注册 FATFS 库，前缀由 base_prefix 变量指定
    此函数旨在使示例代码更加紧凑。
    esp_err_t esp_vfs_fat_spiflash_mount_rw_wl（const char *base_path， const char *partition_label， const esp_vfs_fat_mount_config_t *mount_config， wl_handle_t *wl_handle)

    base_path -- 应挂载 FATFS 分区的路径（例如 “/spiflash”）
    partition_label -- 应该使用的分区的标签
    mount_config -- 指向结构的指针，其中包含用于挂载 FATFS 的额外参数
    wl_handle -- [出] 磨损调平驱动器手柄

    成功ESP_OK
    ESP_ERR_NOT_FOUND分区表是否不包含具有给定标签的 FATFS 分区
    ESP_ERR_INVALID_STATE 如果已调用 esp_vfs_fat_spiflash_mount_rw_wl
    ESP_ERR_NO_MEM 如果无法分配内存
    ESP_FAIL 如果无法挂载分区
    磨损均衡库、SPI 闪存驱动程序或 FATFS 驱动程序中的其他错误代码
 */
#include "spifat.h"

static const char *TAG = "fat";

FRESULT ff_scan_sd(const char *path)
{
    DIR dir;
    FILINFO fno;
    FRESULT res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        while (1)
        {
            res = f_readdir(&dir, &fno);
            if ((res != FR_OK) || (fno.fname[0] == 0))
                break;
            if (fno.fname[0] == '.' && (fno.fname[1] == '\0' || (fno.fname[1] == '.' && fno.fname[2] == '\0')))
                continue;
#if 1
            printf("%s/%-20s %ld bytes\n", path, fno.fname, fno.fsize);
#else
            printf("%s/%s\n", path, fno.fname);
#endif
            if (fno.fattrib & AM_DIR)
            {
                char sub_path[512];
                sprintf(sub_path, "%s/%s", path, fno.fname);
                ff_scan_sd(sub_path);
            }
        }
        f_closedir(&dir);
        return FR_OK;
    }
    else
    {
        printf("%s open fail!\n", path);
        return FR_DISK_ERR;
    }
}

void list_files(const char *path)
{
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL)
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        // 忽略 "." 和 ".." 目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // 构造完整的文件路径
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        // 检查文件类型
        struct stat statbuf;
        if (stat(fullpath, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            // 如果是目录，递归遍历子目录
            printf("Directory: %s\n", fullpath);
            list_files(fullpath);
        }
        else
        {
            // 如果是文件，打印文件路径
            printf("File: %s\n", fullpath);
        }
    }

    closedir(dp);
}

void flashfat_mount(struct FLASHFAT *fat)
{
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount_rw_wl(fat->root_path, fat->partition_label, &fat->mount_config, &fat->wl_handle));

    list_files("/spiflash");
    printf("sdsd\n");
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(fat->root_path, fat->wl_handle));
}
