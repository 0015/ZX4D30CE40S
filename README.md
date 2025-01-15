

# Documentation

Using the latest ESP-IDF environment (v5.5.0) with LVGL v8.

- Components
  - **apps**: Contains application examples.
  - **camera_video**: Example for camera functionality.
  - **custom_board**: Includes board pin configurations and RGB screen driver examples.
  - **esp_lcd_touch_cst3240**: Supports single-point touch. Modifications are required for multi-point touch support.
  - **esp_lvgl_port**: Official Espressif LVGL library. Modified the `esp_lvgl_port_disp.c` file to enable RGB screen usage on P4.
  - **eth_ethernet**: Example for Ethernet functionality.
  - **human_face_detect**: Human face detection example.
  - **pedestrian_detect**: Pedestrian detection example.
  - **sd_card_sdmmc**: Example for SD card functionality.
  - **tusb**: Example for USB functionality.
- Main
  - **brookesia**: Example usage of ESP-Brookesia.

## APP_PanelLAN
The APP primarily displays the initialization status of the following components: **SD Card**, **Ethernet**, **USB**, and **Camera**. The `app_main` function calls the required examples. Currently, there are no additional features displayed.

## RGB Screen
- The screen uses an **RGB565 interface**.
- The screen's touch functionality shares the same **I2C bus** with the camera.