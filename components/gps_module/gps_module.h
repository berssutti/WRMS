#ifndef GPS_MODULE_H
#define GPS_MODULE_H

void parseGPGGA(char *sentence);

void parseNMEA(char *data);

void init_uart(int uart_num, int baud_rate);

void init_gps_uart();

void uart_task(void *pvParameters);

void atualizaErros();

void zeraDistancia();

void calculaDistancia();

#endif