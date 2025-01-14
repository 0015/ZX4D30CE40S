/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "qmsd_board_pin.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

#include "brookesia.h"
#include "sd_card_sdmmc.h"
#include "eth_ethernet.h"
#include "tusb_composite.h"
#include "camera_video.h"
#include "zx4d30ce40s_lcd_rgb.h"

void app_main(void)
{
    zx4d30ce40s_lcd_rgb_init();
    sd_card_sdmmc_demo();
    
    // /* Show LVGL objects */
    lvgl_port_lock(0);
    // app_main_display();
    brookesia_demo_ui(bsp_display_start_with_config(), bsp_display_get_input_dev());
    lvgl_port_unlock();

    eth_init_net();
    tinyusb_task_init();
    // video_test_main(bsp_i2c_get_handle());
}
