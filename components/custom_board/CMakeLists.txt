set(requires esp_lvgl_port esp_lcd_touch_cst3240 esp_driver_i2c)

idf_component_register(
    SRC_DIRS .
    INCLUDE_DIRS .
    REQUIRES ${requires}
)