set(requires custom_board camera_video esp_timer nvs_flash esp_http_server esp_video)

idf_component_register(
    SRC_DIRS .
    INCLUDE_DIRS .
    REQUIRES ${requires}
)