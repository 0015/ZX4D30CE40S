/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_cst3240.h"

static const char *TAG = "CST3240";

/* CST3240 registers */
#define ESP_LCD_TOUCH_CST3240_READ_KEY_REG (0xD005)
#define ESP_LCD_TOUCH_CST3240_READ_XY_REG (0xD000)
#define ESP_LCD_TOUCH_CST3240_CONFIG_REG (0xD1F8)
#define ESP_LCD_TOUCH_CST3240_PRODUCT_ID_REG (0xD200)
#define ESP_LCD_TOUCH_CST3240_ENTER_SLEEP (0xD105)

/* CST3240 support key num */
#define ESP_CST3240_TOUCH_MAX_BUTTONS (4)

/*******************************************************************************
 * Function definitions
 *******************************************************************************/
static esp_err_t esp_lcd_touch_cst3240_read_data(esp_lcd_touch_handle_t tp);
static bool esp_lcd_touch_cst3240_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
#if (CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS > 0)
static esp_err_t esp_lcd_touch_cst3240_get_button_state(esp_lcd_touch_handle_t tp, uint8_t n, uint8_t *state);
#endif
static esp_err_t esp_lcd_touch_cst3240_del(esp_lcd_touch_handle_t tp);

/* I2C read/write */
static esp_err_t touch_cst3240_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);
static esp_err_t touch_cst3240_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data);

/* CST3240 reset */
static esp_err_t touch_cst3240_reset(esp_lcd_touch_handle_t tp);
/* Read status and config register */
static esp_err_t touch_cst3240_read_cfg(esp_lcd_touch_handle_t tp);

/* CST3240 enter/exit sleep mode */
static esp_err_t esp_lcd_touch_cst3240_enter_sleep(esp_lcd_touch_handle_t tp);
static esp_err_t esp_lcd_touch_cst3240_exit_sleep(esp_lcd_touch_handle_t tp);

/*******************************************************************************
 * Public API functions
 *******************************************************************************/

esp_err_t esp_lcd_touch_new_i2c_cst3240(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch)
{
    esp_err_t ret = ESP_OK;

    assert(io != NULL);
    assert(config != NULL);
    assert(out_touch != NULL);

    /* Prepare main structure */
    esp_lcd_touch_handle_t esp_lcd_touch_cst3240 = heap_caps_calloc(1, sizeof(esp_lcd_touch_t), MALLOC_CAP_SPIRAM);
    ESP_GOTO_ON_FALSE(esp_lcd_touch_cst3240, ESP_ERR_NO_MEM, err, TAG, "no mem for CST3240 controller");

    /* Communication interface */
    esp_lcd_touch_cst3240->io = io;

    /* Only supported callbacks are set */
    esp_lcd_touch_cst3240->read_data = esp_lcd_touch_cst3240_read_data;
    esp_lcd_touch_cst3240->get_xy = esp_lcd_touch_cst3240_get_xy;
#if (CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS > 0)
    esp_lcd_touch_cst3240->get_button_state = esp_lcd_touch_cst3240_get_button_state;
#endif
    esp_lcd_touch_cst3240->del = esp_lcd_touch_cst3240_del;
    // esp_lcd_touch_cst3240->enter_sleep = esp_lcd_touch_cst3240_enter_sleep;
    // esp_lcd_touch_cst3240->exit_sleep = esp_lcd_touch_cst3240_exit_sleep;

    /* Mutex */
    esp_lcd_touch_cst3240->data.lock.owner = portMUX_FREE_VAL;

    /* Save config */
    memcpy(&esp_lcd_touch_cst3240->config, config, sizeof(esp_lcd_touch_config_t));
    esp_lcd_touch_io_cst3240_config_t *cst3240_config = (esp_lcd_touch_io_cst3240_config_t *)esp_lcd_touch_cst3240->config.driver_data;

    /* Prepare pin for touch controller reset */
    if (esp_lcd_touch_cst3240->config.rst_gpio_num != GPIO_NUM_NC)
    {
        const gpio_config_t rst_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3240->config.rst_gpio_num)};
        ret = gpio_config(&rst_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");
    }

    if (cst3240_config && esp_lcd_touch_cst3240->config.rst_gpio_num != GPIO_NUM_NC && esp_lcd_touch_cst3240->config.int_gpio_num != GPIO_NUM_NC)
    {
        /* Prepare pin for touch controller int */
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = 0,
            .pull_up_en = 1,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3240->config.int_gpio_num),
        };
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");

        ESP_RETURN_ON_ERROR(gpio_set_level(esp_lcd_touch_cst3240->config.rst_gpio_num, esp_lcd_touch_cst3240->config.levels.reset), TAG, "GPIO set level error!");
        ESP_RETURN_ON_ERROR(gpio_set_level(esp_lcd_touch_cst3240->config.int_gpio_num, 0), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Select I2C addr, set output high or low */
        uint32_t gpio_level;
        if (ESP_LCD_TOUCH_IO_I2C_CST3240_ADDRESS == cst3240_config->dev_addr)
        {
            gpio_level = 0;
        }
        else
        {
            gpio_level = 0;
            ESP_LOGE(TAG, "Addr (0x%X) is invalid", cst3240_config->dev_addr);
        }
        ESP_RETURN_ON_ERROR(gpio_set_level(esp_lcd_touch_cst3240->config.int_gpio_num, gpio_level), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(1));

        ESP_RETURN_ON_ERROR(gpio_set_level(esp_lcd_touch_cst3240->config.rst_gpio_num, !esp_lcd_touch_cst3240->config.levels.reset), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));

        vTaskDelay(pdMS_TO_TICKS(50));
    }
    else
    {
        ESP_LOGW(TAG, "Unable to initialize the I2C address");
        /* Reset controller */
        ret = touch_cst3240_reset(esp_lcd_touch_cst3240);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "CST3240 reset failed");
    }

    /* Prepare pin for touch interrupt */
    if (esp_lcd_touch_cst3240->config.int_gpio_num != GPIO_NUM_NC)
    {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = (esp_lcd_touch_cst3240->config.levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE),
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3240->config.int_gpio_num)};
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");

        /* Register interrupt callback */
        if (esp_lcd_touch_cst3240->config.interrupt_callback)
        {
            esp_lcd_touch_register_interrupt_callback(esp_lcd_touch_cst3240, esp_lcd_touch_cst3240->config.interrupt_callback);
        }
    }

    /* Read status and config info */
    ret = touch_cst3240_read_cfg(esp_lcd_touch_cst3240);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "CST3240 init failed");

err:
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (0x%x)! Touch controller CST3240 initialization failed!", ret);
        if (esp_lcd_touch_cst3240)
        {
            esp_lcd_touch_cst3240_del(esp_lcd_touch_cst3240);
        }
    }

    *out_touch = esp_lcd_touch_cst3240;

    return ret;
}

static esp_err_t esp_lcd_touch_cst3240_enter_sleep(esp_lcd_touch_handle_t tp)
{
    esp_err_t err = touch_cst3240_i2c_write(tp, ESP_LCD_TOUCH_CST3240_ENTER_SLEEP, 0x05);
    ESP_RETURN_ON_ERROR(err, TAG, "Enter Sleep failed!");

    return ESP_OK;
}

static esp_err_t esp_lcd_touch_cst3240_exit_sleep(esp_lcd_touch_handle_t tp)
{
    esp_err_t ret;
    esp_lcd_touch_handle_t esp_lcd_touch_cst3240 = tp;

    if (esp_lcd_touch_cst3240->config.int_gpio_num != GPIO_NUM_NC)
    {
        const gpio_config_t int_gpio_config_high = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3240->config.int_gpio_num)};
        ret = gpio_config(&int_gpio_config_high);
        ESP_RETURN_ON_ERROR(ret, TAG, "High GPIO config failed");
        gpio_set_level(esp_lcd_touch_cst3240->config.int_gpio_num, 1);

        vTaskDelay(pdMS_TO_TICKS(5));

        const gpio_config_t int_gpio_config_float = {
            .mode = GPIO_MODE_OUTPUT_OD,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3240->config.int_gpio_num)};
        ret = gpio_config(&int_gpio_config_float);
        ESP_RETURN_ON_ERROR(ret, TAG, "Float GPIO config failed");
    }

    return ESP_OK;
}

static esp_err_t esp_lcd_touch_cst3240_read_data(esp_lcd_touch_handle_t tp)
{
    esp_err_t err;
    uint8_t buf[41];
    uint8_t touch_cnt = 0;
    uint16_t clear = 0xAB;
    size_t i = 0;

    assert(tp != NULL);

    err = touch_cst3240_i2c_read(tp, ESP_LCD_TOUCH_CST3240_READ_XY_REG, buf, 7);
    ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

    bool touch_active = buf[0] & 0x04;
    if (!touch_active)
    {
        portENTER_CRITICAL(&tp->data.lock);
        tp->data.points = 0; // No touch points active
        portEXIT_CRITICAL(&tp->data.lock);
        return ESP_FAIL;
    }

    uint16_t x = (buf[1] << 4) | ((buf[3] >> 4) & 0x0F);
    uint16_t y = (buf[2] << 4) | (buf[3] & 0x0F);

    touch_cst3240_i2c_write(tp, ESP_LCD_TOUCH_CST3240_READ_XY_REG, 0xAB);
    // printf("%d %d \n", x, y);
    portENTER_CRITICAL(&tp->data.lock);
    tp->data.points = 1; // Assuming CST3240 only reports a single touch point
    tp->data.coords[0].x = x;
    tp->data.coords[0].y = y;
    // CST3240 does not provide strength information in the given code
    portEXIT_CRITICAL(&tp->data.lock);

    return ESP_OK;
}

static bool esp_lcd_touch_cst3240_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    assert(tp != NULL);
    assert(x != NULL);
    assert(y != NULL);
    assert(point_num != NULL);
    assert(max_point_num > 0);

    portENTER_CRITICAL(&tp->data.lock);

    /* Count of points */
    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);

    for (size_t i = 0; i < *point_num; i++)
    {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength)
        {
            strength[i] = tp->data.coords[i].strength;
        }
    }

    /* Invalidate */
    tp->data.points = 0;

    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

#if (CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS > 0)
static esp_err_t esp_lcd_touch_cst3240_get_button_state(esp_lcd_touch_handle_t tp, uint8_t n, uint8_t *state)
{
    esp_err_t err = ESP_OK;
    assert(tp != NULL);
    assert(state != NULL);

    *state = 0;

    portENTER_CRITICAL(&tp->data.lock);

    if (n > tp->data.buttons)
    {
        err = ESP_ERR_INVALID_ARG;
    }
    else
    {
        *state = tp->data.button[n].status;
    }

    portEXIT_CRITICAL(&tp->data.lock);

    return err;
}
#endif

static esp_err_t esp_lcd_touch_cst3240_del(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    /* Reset GPIO pin settings */
    if (tp->config.int_gpio_num != GPIO_NUM_NC)
    {
        gpio_reset_pin(tp->config.int_gpio_num);
        if (tp->config.interrupt_callback)
        {
            gpio_isr_handler_remove(tp->config.int_gpio_num);
        }
    }

    /* Reset GPIO pin settings */
    if (tp->config.rst_gpio_num != GPIO_NUM_NC)
    {
        gpio_reset_pin(tp->config.rst_gpio_num);
    }

    free(tp);

    return ESP_OK;
}

/*******************************************************************************
 * Private API function
 *******************************************************************************/

/* Reset controller */
static esp_err_t touch_cst3240_reset(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    if (tp->config.rst_gpio_num != GPIO_NUM_NC)
    {
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return ESP_OK;
}

static esp_err_t touch_cst3240_read_cfg(esp_lcd_touch_handle_t tp)
{
    uint8_t buf[4];

    assert(tp != NULL);

    ESP_RETURN_ON_ERROR(touch_cst3240_i2c_read(tp, ESP_LCD_TOUCH_CST3240_PRODUCT_ID_REG, (uint8_t *)&buf[0], 3), TAG, "CST3240 read error!");
    ESP_RETURN_ON_ERROR(touch_cst3240_i2c_read(tp, ESP_LCD_TOUCH_CST3240_CONFIG_REG, (uint8_t *)&buf[3], 1), TAG, "CST3240 read error!");

    // ESP_LOGI(TAG, "TouchPad_ID:0x%02x,0x%02x,0x%02x", buf[0], buf[1], buf[2]);
    // ESP_LOGI(TAG, "TouchPad_Config_Version:%d", buf[3]);

    return ESP_OK;
}

static esp_err_t touch_cst3240_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len)
{
    assert(tp != NULL);
    assert(data != NULL);

    /* Read data */
    return esp_lcd_panel_io_rx_param(tp->io, reg, data, len);
}

static esp_err_t touch_cst3240_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data)
{
    assert(tp != NULL);

    // *INDENT-OFF*
    /* Write data */
    return esp_lcd_panel_io_tx_param(tp->io, reg, (uint8_t[]){data}, 1);
    // *INDENT-ON*
}