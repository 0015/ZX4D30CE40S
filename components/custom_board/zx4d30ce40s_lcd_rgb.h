#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "driver/i2c_master.h"

lv_disp_t *bsp_display_start_with_config(void);

lv_indev_t *bsp_display_get_input_dev(void);

i2c_master_bus_handle_t bsp_i2c_get_handle(void);

bool bsp_display_lock(uint32_t timeout_ms);

void bsp_display_unlock(void);

void zx4d30ce40s_lcd_rgb_init(void);

#ifdef __cplusplus
}
#endif