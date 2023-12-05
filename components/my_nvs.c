#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "my_nvs.h"

#define NVS_NAMESPACE "storage"

void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void nvs_write(int32_t valor, char *name)
{
    esp_err_t err;
    nvs_handle_t nvs_handle;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error opening NVS namespace: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_i32(nvs_handle, name, valor);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error writing to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error committing NVS changes: %s", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

int32_t nvs_read(char *name)
{
    esp_err_t err;
    nvs_handle_t nvs_handle;
    int32_t valor = 0;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error opening NVS namespace: %s", esp_err_to_name(err));
        return 0;
    }

    err = nvs_get_i32(nvs_handle, name, &valor);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Error reading 'contador' from NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return 0;
    }
    nvs_close(nvs_handle);

    return valor;
}