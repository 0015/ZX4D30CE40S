/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_brookesia.h"
#include "ui.h"

#include "sd_card_sdmmc.h"
#include "eth_ethernet.h"
#include "tusb_composite.h"
#include "camera_video.h"

LV_FONT_DECLARE(img_panellan)

typedef struct
{
    lv_obj_t *left_cont;
    lv_obj_t *sd_btn;
    lv_obj_t *sd_txt;
    lv_obj_t *eth_btn;
    lv_obj_t *eth_txt;
    lv_obj_t *camera_btn;
    lv_obj_t *camera_txt;
    lv_obj_t *tinyusb_btn;
    lv_obj_t *tinyusb_txt;

    lv_obj_t *right_cont;

    lv_obj_t *sd_cont;
    lv_obj_t *sdc_state_txt;

    lv_obj_t *eth_cont;
    lv_obj_t *eth_state_txt;

    lv_obj_t *camera_cont;
    lv_obj_t *camera_state_txt;

    lv_obj_t *tinyusb_cont;
    lv_obj_t *tinyusb_state_txt;
} screen_main_t;

screen_main_t screen_main;

void page_obj_hide(lv_obj_t *obj)
{
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) == false)
    {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void page_obj_show(lv_obj_t *obj)
{
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) == true)
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void sd_btn_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_text_color(screen_main.sd_txt, lv_color_hex(0x603CF8), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.eth_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.camera_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.tinyusb_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        page_obj_show(screen_main.sd_cont);
        page_obj_hide(screen_main.eth_cont);
        page_obj_hide(screen_main.camera_cont);
        page_obj_hide(screen_main.tinyusb_cont);
    }
}

static void eth_btn_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_text_color(screen_main.sd_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.eth_txt, lv_color_hex(0x603CF8), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.camera_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.tinyusb_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        page_obj_show(screen_main.eth_cont);
        page_obj_hide(screen_main.sd_cont);
        page_obj_hide(screen_main.camera_cont);
        page_obj_hide(screen_main.tinyusb_cont);
    }
}

static void camera_btn_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_text_color(screen_main.sd_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.eth_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.camera_txt, lv_color_hex(0x603CF8), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.tinyusb_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        page_obj_show(screen_main.camera_cont);
        page_obj_hide(screen_main.sd_cont);
        page_obj_hide(screen_main.eth_cont);
        page_obj_hide(screen_main.tinyusb_cont);
    }
}

static void tinyusb_btn_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_text_color(screen_main.sd_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.eth_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.camera_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(screen_main.tinyusb_txt, lv_color_hex(0x603CF8), LV_PART_MAIN | LV_STATE_DEFAULT);
        page_obj_hide(screen_main.camera_cont);
        page_obj_hide(screen_main.sd_cont);
        page_obj_hide(screen_main.eth_cont);
        page_obj_show(screen_main.tinyusb_cont);
    }
}

static void screen_main_left_cont(lv_obj_t *parent)
{
    screen_main.left_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.left_cont, LV_PCT(25), LV_PCT(90));
    lv_obj_align(screen_main.left_cont, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_opa(screen_main.left_cont, 150, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main.sd_btn = lv_obj_create(screen_main.left_cont);
    lv_obj_set_size(screen_main.sd_btn, LV_PCT(100), LV_PCT(20));
    screen_main.sd_txt = lv_label_create(screen_main.sd_btn);
    lv_label_set_text(screen_main.sd_txt, "SD CARD");
    lv_obj_center(screen_main.sd_txt);
    lv_obj_add_event_cb(screen_main.sd_btn, sd_btn_event, LV_EVENT_ALL, NULL);

    screen_main.eth_btn = lv_obj_create(screen_main.left_cont);
    lv_obj_set_size(screen_main.eth_btn, LV_PCT(100), LV_PCT(20));
    screen_main.eth_txt = lv_label_create(screen_main.eth_btn);
    lv_label_set_text(screen_main.eth_txt, "ETH");
    lv_obj_center(screen_main.eth_txt);
    lv_obj_add_event_cb(screen_main.eth_btn, eth_btn_event, LV_EVENT_ALL, NULL);

    screen_main.camera_btn = lv_obj_create(screen_main.left_cont);
    lv_obj_set_size(screen_main.camera_btn, LV_PCT(100), LV_PCT(20));
    screen_main.camera_txt = lv_label_create(screen_main.camera_btn);
    lv_label_set_text(screen_main.camera_txt, "CAMERA");
    lv_obj_center(screen_main.camera_txt);
    lv_obj_add_event_cb(screen_main.camera_btn, camera_btn_event, LV_EVENT_ALL, NULL);

    screen_main.tinyusb_btn = lv_obj_create(screen_main.left_cont);
    lv_obj_set_size(screen_main.tinyusb_btn, LV_PCT(100), LV_PCT(20));
    screen_main.tinyusb_txt = lv_label_create(screen_main.tinyusb_btn);
    lv_label_set_text(screen_main.tinyusb_txt, "TinyUSB");
    lv_obj_center(screen_main.tinyusb_txt);
    lv_obj_add_event_cb(screen_main.tinyusb_btn, tinyusb_btn_event, LV_EVENT_ALL, NULL);

    lv_obj_set_flex_flow(screen_main.left_cont, LV_FLEX_FLOW_COLUMN);
}

static void screen_main_sd_cont(lv_obj_t *parent)
{
    screen_main.sd_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.sd_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(screen_main.sd_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(screen_main.sd_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main.sdc_state_txt = lv_label_create(screen_main.sd_cont);
    lv_label_set_text(screen_main.sdc_state_txt, "");
}

static void screen_main_eth_cont(lv_obj_t *parent)
{
    screen_main.eth_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.eth_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(screen_main.eth_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(screen_main.eth_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main.eth_state_txt = lv_label_create(screen_main.eth_cont);
    lv_label_set_text(screen_main.eth_state_txt, "");
}

static void screen_main_camera_cont(lv_obj_t *parent)
{
    screen_main.camera_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.camera_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(screen_main.camera_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(screen_main.camera_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main.camera_state_txt = lv_label_create(screen_main.camera_cont);
    lv_label_set_text(screen_main.camera_state_txt, "");
}

static void screen_main_tinyusb_cont(lv_obj_t *parent)
{
    screen_main.tinyusb_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.tinyusb_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(screen_main.tinyusb_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(screen_main.tinyusb_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main.tinyusb_state_txt = lv_label_create(screen_main.tinyusb_cont);
    lv_label_set_text(screen_main.tinyusb_state_txt, "");
}

static void screen_main_right_cont(lv_obj_t *parent)
{
    screen_main.right_cont = lv_obj_create(parent);
    lv_obj_set_size(screen_main.right_cont, LV_PCT(70), LV_PCT(90));
    lv_obj_align(screen_main.right_cont, LV_ALIGN_LEFT_MID, LV_PCT(25) + 2, 0);
    lv_obj_set_style_bg_opa(screen_main.right_cont, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(screen_main.right_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    screen_main_sd_cont(screen_main.right_cont);
    screen_main_eth_cont(screen_main.right_cont);
    screen_main_camera_cont(screen_main.right_cont);
    screen_main_tinyusb_cont(screen_main.right_cont);

    page_obj_hide(screen_main.sd_cont);
    page_obj_hide(screen_main.eth_cont);
    page_obj_hide(screen_main.camera_cont);
    page_obj_hide(screen_main.tinyusb_cont);
}

void screen_main_sd_card_status()
{
    if (lv_obj_has_flag(screen_main.sd_cont, LV_OBJ_FLAG_HIDDEN) == true)
    {
        return;
    }
    uint8_t status = sd_card_sdmmc_get_status();
    if (status == 0)
    {
        lv_label_set_text(screen_main.sdc_state_txt, "Please wait while the SD card is being initialized.");
        lv_obj_set_style_text_color(screen_main.sdc_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else if (status == 1)
    {
        lv_label_set_text(screen_main.sdc_state_txt, "SD card initialization successful.");
        lv_obj_set_style_text_color(screen_main.sdc_state_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else if (status == 2)
    {
        lv_label_set_text(screen_main.sdc_state_txt, "Failed to mount filesystem. \nIf you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        lv_obj_set_style_text_color(screen_main.sdc_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else if (status == 3)
    {
        lv_label_set_text(screen_main.sdc_state_txt, "Failed to initialize the card. \nMake sure SD card lines have pull-up resistors in place.");
        lv_obj_set_style_text_color(screen_main.sdc_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void screen_main_eth_status()
{
    if (lv_obj_has_flag(screen_main.eth_cont, LV_OBJ_FLAG_HIDDEN) == true)
    {
        return;
    }
    if (eth_net_get_status() == false)
    {
        lv_label_set_text(screen_main.eth_state_txt, "Please check your network connection.");
        lv_obj_set_style_text_color(screen_main.eth_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        lv_label_set_text_fmt(screen_main.eth_state_txt, "Network connection successful.\n\nIP:%d.%d.%d.%d", IP2STR(&eth_net_get_ip()->ip));
        lv_obj_set_style_text_color(screen_main.eth_state_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void screen_main_tinyusb_status()
{
    if (lv_obj_has_flag(screen_main.tinyusb_cont, LV_OBJ_FLAG_HIDDEN) == true)
    {
        return;
    }
    if (tinyusb_get_init_status() == false)
    {
        lv_label_set_text(screen_main.tinyusb_state_txt, "USB is initializing, please wait.");
        lv_obj_set_style_text_color(screen_main.tinyusb_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        lv_label_set_text(screen_main.tinyusb_state_txt, "USB Composite initialization DONE.");
        lv_obj_set_style_text_color(screen_main.tinyusb_state_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void screen_main_camera_status()
{
    if (lv_obj_has_flag(screen_main.camera_cont, LV_OBJ_FLAG_HIDDEN) == true)
    {
        return;
    }
    if (camera_video_get_status() == 0)
    {
        lv_label_set_text(screen_main.camera_state_txt, "Waiting for the camera to initialize.");
        lv_obj_set_style_text_color(screen_main.camera_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else if (camera_video_get_status() == 1)
    {
        lv_label_set_text(screen_main.camera_state_txt, "Camera initialization successful.");
        lv_obj_set_style_text_color(screen_main.camera_state_txt, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        lv_label_set_text(screen_main.camera_state_txt, "Camera initialization failed, please check the connection to the camera.");
        lv_obj_set_style_text_color(screen_main.camera_state_txt, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void screen_main_timer_cb(lv_timer_t *timer)
{
    screen_main_sd_card_status();
    screen_main_eth_status();
    screen_main_tinyusb_status();
    screen_main_camera_status();
}

bool phone_app_panellan_ui_init(void)
{
    lv_obj_t *logo = lv_img_create(lv_scr_act());
    lv_img_set_src(logo, &img_panellan);
    lv_obj_center(logo);

    screen_main_left_cont(lv_scr_act());
    screen_main_right_cont(lv_scr_act());

    lv_timer_create(screen_main_timer_cb, 500, NULL);

    return true;
}
