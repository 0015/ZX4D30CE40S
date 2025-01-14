set(requires nvs_flash esp_eth esp-tls esp_netif esp_event efuse)

idf_component_register(
    SRC_DIRS .
    INCLUDE_DIRS .
    REQUIRES ${requires}
)