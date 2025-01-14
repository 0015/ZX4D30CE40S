set(requires custom_board fatfs wear_levelling esp_partition usb)

idf_component_register(
    SRC_DIRS .
    INCLUDE_DIRS .
    REQUIRES ${requires}
)