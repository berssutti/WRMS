#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "../components/my_nvs/my_nvs.h"
#include "../components/wifi/wifi.h"
#include "../components/mqtt/mqtt.h"
#include "../components/gps_module/gps_module.h"

#include "driver/gpio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#define NVS_NAMESPACE "storage"

#define GPIO_OUTPUT_IO_0 2
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_IO_0)

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;
float velocidade = 0;
float altitude = 0;
float latitude = 0;
float longitude = 0;
int wifi_connected = 0;
int mqtt_connected = 0;



void conectadoWifi(void *params)
{
    while (!mqtt_connected)
    {
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
        {

            mqtt_start();
        }
    }

    vTaskDelete(NULL);
}

void conectadoMQTT(void *params)
{
    srand(time(NULL));

    while (true)
    {
        printf("Esperando conexão com MQTT\n");
        if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
        {
            printf("Chegou conexão com MQTT\n");
            char *mensagem = malloc(sizeof(char) * 100);
            sprintf(mensagem, "{\"speed\": %f, \"altitud\": %f, \"latitude\": %f, \"longitude\": %f}", velocidade, altitude, latitude, longitude);
            mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
            xSemaphoreGive(conexaoMQTTSemaphore);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    while (true)
    {
        int cnt = 0;
        while ((!wifi_connected))
        {
            cnt++;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        }

        cnt = 0;
        while ((!mqtt_connected))
        {
            cnt++;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        }

        gpio_set_level(GPIO_OUTPUT_IO_0, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
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
    init_gps_uart();
    xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&conectadoMQTT, "Envio de Dados do GPS", 4096, NULL, 1, NULL);
    xTaskCreate(&uart_task, "Task que fica monitorando dados vindos do GPS", 4069, NULL, 1, NULL);
}
