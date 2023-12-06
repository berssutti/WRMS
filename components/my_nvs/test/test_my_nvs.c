#include <unity.h>
#include "../my_nvs.h"

TEST_CASE("Store in nvs", "[my_nvs]")
{
    const int32_t numero = 10;
    TEST_ASSERT_EQUAL(ESP_OK, nvs_write(numero, "numero"));
}

TEST_CASE("Read from nvs", "[my_nvs]")
{
    const int32_t numero = 10;
    nvs_write(numero, "numero");
    TEST_ASSERT_EQUAL(ESP_OK, nvs_read("numero"));
}