/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "phone_app_panellan.hpp"
#include "phone_app_panellan_main.h"
#include "lvgl.h"
LV_FONT_DECLARE(img_logo);

using namespace std;

PhoneAppPanelLan::PhoneAppPanelLan(bool use_status_bar, bool use_navigation_bar):
    ESP_Brookesia_PhoneApp("PanelLan", &img_logo, true, use_status_bar, use_navigation_bar)
{
}

PhoneAppPanelLan::PhoneAppPanelLan():
    ESP_Brookesia_PhoneApp("PanelLan", &img_logo, true)
{
}

PhoneAppPanelLan::~PhoneAppPanelLan()
{
    ESP_BROOKESIA_LOGD("Destroy(@0x%p)", this);
}

bool PhoneAppPanelLan::run(void)
{
    ESP_BROOKESIA_LOGD("Run");

    // Create all UI resources here
    ESP_BROOKESIA_CHECK_FALSE_RETURN(phone_app_panellan_main_init(), false, "Main init failed");

    return true;
}

bool PhoneAppPanelLan::back(void)
{
    ESP_BROOKESIA_LOGD("Back");

    // If the app needs to exit, call notifyCoreClosed() to notify the core to close the app
    ESP_BROOKESIA_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");

    return true;
}

// bool PhoneAppPanelLan::close(void)
// {
//     ESP_BROOKESIA_LOGD("Close");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppPanelLan::init()
// {
//     ESP_BROOKESIA_LOGD("Init");

//     /* Do some initialization here if needed */

//     return true;
// }

// bool PhoneAppPanelLan::deinit()
// {
//     ESP_BROOKESIA_LOGD("Deinit");

//     /* Do some deinitialization here if needed */

//     return true;
// }

// bool PhoneAppPanelLan::pause()
// {
//     ESP_BROOKESIA_LOGD("Pause");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppPanelLan::resume()
// {
//     ESP_BROOKESIA_LOGD("Resume");

//     /* Do some operations here if needed */

//     return true;
// }

// bool PhoneAppPanelLan::cleanResource()
// {
//     ESP_BROOKESIA_LOGD("Clean resource");

//     /* Do some cleanup here if needed */

//     return true;
// }
