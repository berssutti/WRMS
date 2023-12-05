#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define TXD_PIN 27
#define RXD_PIN 25
#define BUF_SIZE 1024

#define NVS_NAMESPACE "storage"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;


void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void uart_task(void *pvParameters) {
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        printf("Length: %d\n", len);
        
        if (len > 0) {
            // Processar os dados lidos do UART aqui
            printf("Recebido: %.*s\n", len, data);
        }
    }
}


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
    printf("Resultado: %d", contador); // é para ser 10

    // printf("Ola mundo");
    // init_uart();
    // xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
}
