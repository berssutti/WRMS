#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "../components/mqtt/mqtt.h"
#include "../components/my_nvs/my_nvs.h"
#include "../components/wifi/wifi.h"

#define TXD_PIN 27
#define RXD_PIN 25
#define BUF_SIZE 1024

#define NVS_NAMESPACE "storage"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;


void conectadoWifi(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
        {
            // Processamento Internet
            mqtt_start();
        }
    }
}


void app_main() {
    // inicializações
    nvs_init();

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);

    // Tests
    int32_t contador=0;
    for(int i=0; i<10;i++){
        contador++;
    }    
    nvs_write(contador, "contador");

    int32_t resultado = nvs_read("contador");
    printf("Resultado: %ld", resultado); // é para ser 10
    
}
