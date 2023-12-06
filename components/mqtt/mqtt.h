#ifndef MQTT_H
#define MQTT_H

esp_err_t mqtt_start();

void mqtt_envia_mensagem(char * topico, char * mensagem);

#endif