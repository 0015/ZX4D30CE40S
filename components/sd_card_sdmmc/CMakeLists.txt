set(requires custom_board esp_driver_sdmmc fatfs)

idf_component_register(
    SRC_DIRS .
    INCLUDE_DIRS .
    REQUIRES ${requires}
)
