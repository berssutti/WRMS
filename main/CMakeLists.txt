idf_component_register(SRCS "main.c"
                    "../components/gpsModule/gpsModule.c"
                    "../components/mqtt.c"
                    "../components/wifi.c"
                    "../components/my_nvs.c"
                    INCLUDE_DIRS ".")
