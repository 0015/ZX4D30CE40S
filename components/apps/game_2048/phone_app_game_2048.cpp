/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "phone_app_game_2048.hpp"
#include "phone_app_game_2048_main.h"
#include "lvgl.h"
LV_FONT_DECLARE(img_logo_2048);

using namespace std;

PhoneAppGame2048::PhoneAppGame2048(bool use_status_bar, bool use_navigation_bar):
    ESP_Brookesia_PhoneApp("Game2048", &img_logo_2048, true, use_status_bar, use_navigation_bar)
{
}

PhoneAppGame2048::PhoneAppGame2048():
    ESP_Brookesia_PhoneApp("Game2048", &img_logo_2048, true)
{
}

PhoneAppGame2048::~PhoneAppGame2048()
{
    ESP_BROOKESIA_LOGD("Destroy(@0x%p)", this);
}

bool PhoneAppGame2048::run(void)
{
    ESP_BROOKESIA_LOGD("Run");

    // Create all UI resources here
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone_app_game_2048_main_init(), false, "Main init failed");

    return true;
}

bool PhoneAppGame2048::back(void)
{
    ESP_BROOKESIA_LOGD("Back");

    // If the app needs to exit, call notifyCoreClosed() to notify the core to close the app
    ESP_BROOKESIA_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");

    return true;
}

// bool PhoneAppGame2048::close(void)
// {
//     ESP_BROOKESIA_LOGD("Close");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppGame2048::init()
// {
//     ESP_BROOKESIA_LOGD("Init");

//     /* Do some initialization here if needed */

//     return true;
// }

// bool PhoneAppGame2048::deinit()
// {
//     ESP_BROOKESIA_LOGD("Deinit");

//     /* Do some deinitialization here if needed */

//     return true;
// }

// bool PhoneAppGame2048::pause()
// {
//     ESP_BROOKESIA_LOGD("Pause");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppGame2048::resume()
// {
//     ESP_BROOKESIA_LOGD("Resume");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppGame2048::cleanResource()
// {
//     ESP_BROOKESIA_LOGD("Clean resource");

//     /* Do some cleanup here if needed */

//     return true;
// }
