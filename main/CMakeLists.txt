idf_component_register(SRCS "main.c"
                    "../components/gps_module/gps_module.c"
                    "../components/mqtt/mqtt.c"
                    "../components/wifi/wifi.c"
                    "../components/my_nvs/my_nvs.c"
                    INCLUDE_DIRS ".")
