#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_netif.h"
#include "esp_eth.h"

esp_netif_ip_info_t* eth_net_get_ip();

bool eth_net_get_status();

void eth_init_net();

#ifdef __cplusplus
}
#endif