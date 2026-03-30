idf_component_register(
    SRCS "rgb_led.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio
    PRIV_REQUIRES led_strip
)
