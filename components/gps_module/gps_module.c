#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "../tinyGPS/tinygps.h"

#define BUF_SIZE 1024
char *gpsStream;

extern float velocidade, altitude, longitude, latitude;

void init_uart(int uart_num, int baud_rate)
{
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void init_gps_uart()
{
    // Configurar UART para o módulo Neo 6M GPS Tracker
    int gps_uart_num = UART_NUM_1; // Escolha o número de UART desejado
    int gps_baud_rate = 9600;      // Taxa de baud para o módulo Neo 6M

    init_uart(gps_uart_num, gps_baud_rate);
}

void uart_task(void *pvParameters)
{
    gpsStream = (char *)malloc(BUF_SIZE);

    while (1)
    {
        printf("Lendo Dados do GPS...\n");
        int len = uart_read_bytes(UART_NUM_1, gpsStream, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            unsigned long fix_age;
            // Processar os dados lidos do UART aqui
            printf("Dados lidos: %.*s\n", len, gpsStream);
            for (int i = 0; i < len; i++)
            {
                gps_encode(gpsStream[i]);
            }
            velocidade = gps_f_speed_mps();
            altitude = gps_f_altitude();
            gps_get_position(&latitude, &longitude, &fix_age);
        }
        else
        {
            printf("Nenhum dado lido do GPS\n");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}