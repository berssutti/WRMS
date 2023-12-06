#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "../components/my_nvs/my_nvs.h"
#include "../components/wifi/wifi.h"
#include "../components/mqtt/mqtt.h"
#include "driver/gpio.h"

#define NVS_NAMESPACE "storage"

#define GPIO_OUTPUT_IO_0 2
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_IO_0)

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

int wifi_connected = 0;
int mqtt_connected = 0;

void conectadoWifi(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
        {
            printf("Pegou o semaforo\n");
            wifi_connected = 1;
            // Processamento Internet
            if (mqtt_start() == ESP_OK)
            {
                mqtt_connected = 1;
            } else {
                printf("Erro ao iniciar MQTT\n");
            }
        }
    }
}

void configurar_led_status_wifi()
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    gpio_config(&io_conf);
}

void blink_led()
{
    int cnt = 0;
    while ((!wifi_connected) || cnt < 5)
    {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
    }

    cnt = 0;
    while ((!mqtt_connected) || cnt < 5)
    {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
    }

    gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    vTaskDelete(NULL);
}

void app_main()
{
    // inicializações
    nvs_init();

    configurar_led_status_wifi();

    xTaskCreate(&blink_led, "Aguardando Conexão com WIFi", 4096, NULL, 1, NULL);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
}
