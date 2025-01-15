# 说明文档

使用esp-idf最新环境(5.5.0),lvgl8

- components
  - apps    (放置APP)
  - camera_video    (摄像头示例)
  - custom_board    (板子引脚，RGB屏幕驱动示例)
  - esp_lcd_touch_cst3240 (只做了单点触摸，多点需要修改此驱动)
  - esp_lvgl_port   (乐鑫官方库，修改了`esp_lvgl_port_disp.c`文件，让P4也能使用RGB屏幕)
  - eth_ethernet    (网口示例)
  - human_face_detect
  - pedestrian_detect
  - sd_card_sdmmc   (SD卡示例)
  - tusb            (USB示例)
- main
  - brookesia       (ESP-Brookesia使用示例)

## APP_PanelLAN
APP里面主要是显示**SD卡**，**网口**，**USB**，**摄像头**是否初始化成功状态，`app_main`函数中调用需要的示例，目前没有其他展示功能

## RGB 屏幕
- 屏幕使用的为RGB565的接口
- 屏幕触摸与摄像头共用一个I2C
