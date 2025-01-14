/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP LCD touch: CST3240
 */

#pragma once

#include "esp_lcd_touch.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Create a new CST3240 touch driver
     *
     * @note The I2C communication should be initialized before use this function.
     *
     * @param io LCD/Touch panel IO handle
     * @param config: Touch configuration
     * @param out_touch: Touch instance handle
     * @return
     *      - ESP_OK                    on success
     *      - ESP_ERR_NO_MEM            if there is no memory for allocating main structure
     */
    esp_err_t esp_lcd_touch_new_i2c_cst3240(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch);

/**
 * @brief I2C address of the CST3240 controller
 *
 * @note When power-on detects low level of the interrupt gpio, address is 0x5D.
 * @note Interrupt gpio is high level, address is 0x14.
 *
 */
#define ESP_LCD_TOUCH_IO_I2C_CST3240_ADDRESS (0x5a)

    /**
     * @brief CST3240 Configuration Type
     *
     */
    typedef struct
    {
        uint8_t dev_addr; /*!< I2C device address */
    } esp_lcd_touch_io_cst3240_config_t;

/**
 * @brief Touch IO configuration structure
 *
 */
#define ESP_LCD_TOUCH_IO_I2C_CST3240_CONFIG()             \
    {                                                     \
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST3240_ADDRESS, \
        .control_phase_bytes = 1,                         \
        .dc_bit_offset = 0,                               \
        .lcd_cmd_bits = 16,                               \
        .flags =                                          \
        {                                                 \
            .disable_control_phase = 1,                   \
        }                                                 \
    }

#ifdef __cplusplus
}
#endif