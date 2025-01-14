#pragma once

#include "driver/i2c_master.h"

void video_test_main(i2c_master_bus_handle_t i2c_handle);

uint8_t camera_video_get_status();