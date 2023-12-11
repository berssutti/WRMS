

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"
#include "../gps_module/gps_module.h"

#define TAG "MQTT"
#define THINGSBOARD_HOST CONFIG_ESP_MQTT_HOST_IP
#define CREDENTIALS_MQTT CONFIG_ESP_MQTT_TOKEN
extern SemaphoreHandle_t conexaoMQTTSemaphore;
extern int wifi_connected;
extern int mqtt_connected;
esp_mqtt_client_handle_t client;
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, (int)event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        mqtt_connected = 1;
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
        xSemaphoreGive(conexaoMQTTSemaphore);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_connected = 0;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strstr(event->data, "zerarDistancia") != NULL)
        {
            zeraDistancia();
        }
        else if (strstr(event->data, "zerarAtributos") != NULL)
        {
            atualizaErros();
        }
        else if (strstr(event->data, "calcularDistancia") != NULL)
        {
            calculaDistancia();
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);

            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
esp_err_t mqtt_start()
{
    char str[80];
    strcpy(str, "mqtt://");
    strcat(str, THINGSBOARD_HOST);
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = str,
        .credentials.username = CREDENTIALS_MQTT

    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    return esp_mqtt_client_start(client);
}
void mqtt_envia_mensagem(char *topico, char *mensagem)
{
    if (mqtt_connected)
    {
        printf("Enviando mensagem: %s para topico %s\n", mensagem, topico);
        int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
        ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
    }
}