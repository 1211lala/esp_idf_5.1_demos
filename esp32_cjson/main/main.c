#include "datafile.h"
#include "spifat.h"
#include "sd_spi_fat.h"

#include "cJSON.h"

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

/*
{
    "ssid": "MyWiFi",
    "password": "password123",
    "port": 2000,
    "IntArray": [
        51,
        15,
        63,
        22,
        96
    ],
    "ip_info": {
        "ip": "192.168.1.100",
        "gateway": "192.168.1.1",
        "subnet": "255.255.255.0"
    },
    "info": [
        {
            "name": "liuao",
            "year": 18
        },
        {
            "name": "xiaoming",
            "year": 18
        }
    ]
}
 */
void parse_json_file(char *file_path)
{
    FILE *f = fopen(file_path, "r");
    if (f == NULL)
    {
        ESP_LOGE("parse_json_file", "%s open fail", file_path);
        return;
    }
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *josn_str = malloc(file_size + 1);
    if (file_size != fread(josn_str, sizeof(char), file_size, f))
    {
        ESP_LOGE("parse_json_file", "fread error");
    }
    josn_str[file_size] = '\0';
    printf("%d json_str size : %s\n", file_size, josn_str);
    fclose(f);

    cJSON *JsonRoot = cJSON_Parse(josn_str);
    if (JsonRoot == NULL)
    {
        ESP_LOGE("parse_json_file", "%s not is josn", josn_str);
        return;
    }
    cJSON *JsonAddress = NULL;
    /* 解析ssid */
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "ssid")))
    {
        if (cJSON_IsString(JsonAddress))
        {
            printf("ssid: %s\n", JsonAddress->valuestring);
        }
        else
        {
            ESP_LOGW("parse_json_file", "ssid 属性不是json");
        }
    }
    else
    {
        ESP_LOGW("parse_json_file", "ssid 属性不存在");
    }
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "password")))
    {
        if (cJSON_IsString(JsonAddress))
        {
            printf("password: %s\n", JsonAddress->valuestring);
        }
    }
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "port")))
    {
        if (cJSON_IsNumber(JsonAddress))
        {
            printf("port: %d\n", JsonAddress->valueint);
        }
    }
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "IntArray")))
    {
        if (cJSON_IsArray(JsonAddress))
        {
            int size = cJSON_GetArraySize(JsonAddress);
            for (uint8_t i = 0; i < size; i++)
            {
                printf("IntArray[%d] = %d\r\n", i, cJSON_GetArrayItem(JsonAddress, i)->valueint);
            }
        }
    }
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "ip_info")))
    {
        cJSON *addr = NULL;
        if (cJSON_IsObject(JsonAddress))
        {

            addr = cJSON_GetObjectItem(JsonAddress, "ip");
            printf("ip_info->ip: %s\n", addr->valuestring);

            addr = cJSON_GetObjectItem(JsonAddress, "gateway");
            printf("ip_info->gateway: %s\n", addr->valuestring);

            addr = cJSON_GetObjectItem(JsonAddress, "subnet");
            printf("ip_info->subnet: %s\n", addr->valuestring);
        }
    }
    if (NULL != (JsonAddress = cJSON_GetObjectItem(JsonRoot, "info")))
    {

        if (cJSON_IsArray(JsonAddress))
        {
            int len = cJSON_GetArraySize(JsonAddress);
            cJSON *addr = NULL;
            cJSON *addrdata = NULL;
            for (int i = 0; i < len; i++)
            {
                addr = cJSON_GetArrayItem(JsonAddress, i);
                addrdata = cJSON_GetObjectItem(addr, "name");
                printf("info[%d]->name: %s\n", i, addrdata->valuestring);
                addrdata = cJSON_GetObjectItem(addr, "year");
                printf("info[%d]->year: %d\n", i, addrdata->valueint);
            }
        }
    }
    cJSON_Delete(JsonRoot);
    free(josn_str);
}

void create_parse_josn(void)
{
    /***************************************构建数据************************************************/
    // 创建父对象
    cJSON *pRoot = cJSON_CreateObject();
    // 在父对象根目录添加字符串数据
    cJSON_AddStringToObject(pRoot, "ssid", "Xiaomi_4C");
    cJSON_AddStringToObject(pRoot, "password", "121314liuAO#");
    // 在父对象根目录添加整型数据
    cJSON_AddNumberToObject(pRoot, "port", 2000);
    // 在父对象根目录添加整型数组
    int array[5] = {51, 15, 63, 22, 96};
    cJSON *pIntArray = cJSON_CreateIntArray(array, 5);
    cJSON_AddItemToObject(pRoot, "IntArray", pIntArray);
    // 在父对象根目录添加子对象
    cJSON *object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "ip", "192.168.10.100");
    cJSON_AddStringToObject(object, "gateway", "192.168.10.1");
    cJSON_AddStringToObject(object, "subnet", "192.168.10.1");
    cJSON_AddItemToObject(pRoot, "ip_info", object);
    // 在父对象根目录添JSON数组,并在数组中新建两个子对象
    cJSON *jsonArray = cJSON_CreateArray();
    cJSON *pArray1 = cJSON_CreateObject();
    cJSON_AddStringToObject(pArray1, "name", "liuao");
    cJSON_AddNumberToObject(pArray1, "year", 18);
    cJSON_AddItemToArray(jsonArray, pArray1);
    cJSON *pArray2 = cJSON_CreateObject();
    cJSON_AddStringToObject(pArray2, "name", "xiaoming");
    cJSON_AddNumberToObject(pArray2, "year", 18);
    cJSON_AddItemToArray(jsonArray, pArray2);
    cJSON_AddItemToObject(pRoot, "info", jsonArray);
    // 获取格式化好的JSON字符串
    char *Json_string = cJSON_Print(pRoot);
    printf("%s\n", Json_string);

    /***************************************解析数据************************************************/
    // 首先整体判断是否为一个json格式的数据
    cJSON *pJsonAddress = NULL;
    cJSON *pJsonRoot = cJSON_Parse(Json_string);
    // 如果是否json格式数据
    if (pJsonRoot != NULL)
    {
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "ssid");
        if (!pJsonAddress)
            ESP_LOGE("JSON", "JSON中没有此对象");
        else
        { // 判断JSON的对象是否为String
            if (cJSON_IsString(pJsonAddress))
            {
                printf("ssid: %s\r\n", pJsonAddress->valuestring);
            }
        } // 下面不再做错误判断
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "password");
        printf("password: %s\r\n", pJsonAddress->valuestring);
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "port");
        printf("port: %d\r\n", pJsonAddress->valueint);
        //
        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "IntArray");
        if (!pJsonAddress)
            return;
        else
        {
            int len = cJSON_GetArraySize(pJsonAddress);
            for (uint8_t i = 0; i < len; i++)
            {
                printf("IntArray[%d] = %d\r\n", i, cJSON_GetArrayItem(pJsonAddress, i)->valueint);
            }
        }

        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "ip_info");
        if (!pJsonAddress)
            return;
        else
        {
            cJSON *address = cJSON_GetObjectItem(pJsonAddress, "ip");
            if (cJSON_IsString(address))
            {
                printf("ip: %s\r\n", address->valuestring);
            } // 不再做数据类型判断
            address = cJSON_GetObjectItem(pJsonAddress, "gateway");
            printf("gateway: %s\r\n", address->valuestring);
            address = cJSON_GetObjectItem(pJsonAddress, "subnet");
            printf("subnet: %s\r\n", address->valuestring);
        }

        pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "info");
        if (!pJsonAddress)
            return;
        else
        {
            cJSON *jsonAddress;
            cJSON *jsonData;
            int len = cJSON_GetArraySize(pJsonAddress);
            printf("JSON共有%d数据对象\r\n", len);
            for (uint8_t i = 0; i < len; i++)
            {
                jsonAddress = cJSON_GetArrayItem(pJsonAddress, i);

                jsonData = cJSON_GetObjectItem(jsonAddress, "name");
                printf("name: %s\r\n", jsonData->valuestring);
                jsonData = cJSON_GetObjectItem(jsonAddress, "year");
                printf("year: %d\r\n", jsonData->valueint);
            }
        }
    }
    cJSON_free((void *)Json_string);
    cJSON_Delete(pRoot);
}
void fatfs_test(void *arg)
{
    flashfat_mount(&flashfat);
    create_parse_josn();
    parse_json_file("/flashroot/data.json");
    while (1)
    {

        ESP_LOGI(" ", "%.2fKbyte", (float)esp_get_free_heap_size() / 1000.0);
        vTaskDelay(1000 / portTICK);
    }
}

void app_main()
{
    xTaskCreate(fatfs_test, "fatfs_test", 1024 * 5, NULL, 5, NULL);
    vTaskDelete(NULL);
}
