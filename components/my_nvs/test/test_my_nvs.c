#include <unity.h>
#include "../my_nvs.h"

TEST_CASE("Store in nvs", "[my_nvs]")
{
    const int32_t numero = 10;
    TEST_ASSERT_EQUAL(0, nvs_write(numero, "numero"));
}