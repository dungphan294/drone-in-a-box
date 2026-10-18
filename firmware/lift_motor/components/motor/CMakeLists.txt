idf_component_register(SRCS "motor.c"
                    INCLUDE_DIRS "include"
                    REQUIRES command esp_driver_gpio esp_driver_ledc)
