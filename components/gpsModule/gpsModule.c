#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "ky033.h"

// configuração do pino do sensor
#define ENTRADA_PIN CONFIG_ESP_KY033_ENTRADA_PIN
#define SAIDA_PIN CONFIG_ESP_KY033_SAIDA_PIN

void ky033_init(gpio_num_t pin)
{
    if (pin == CONFIG_ESP_KY033_ENTRADA_PIN || pin == CONFIG_ESP_KY033_SAIDA_PIN)
    {
        gpio_config_t io_conf = {
            .pin_bit_mask = ((1ULL << ENTRADA_PIN) | ((1ULL << SAIDA_PIN))),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_NEGEDGE};
        gpio_config(&io_conf);
    }
}