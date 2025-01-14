// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.3
// LVGL version: 8.3.6
// Project name: Camera

#include "../ui.h"

void ui_ScreenCameraShot_screen_init(void)
{
ui_ScreenCameraShot = lv_obj_create(NULL);
lv_obj_clear_flag( ui_ScreenCameraShot, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_flex_flow(ui_ScreenCameraShot,LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(ui_ScreenCameraShot, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
lv_obj_set_style_bg_color(ui_ScreenCameraShot, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ScreenCameraShot, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_ScreenCameraShot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_PanelCameraShotTitle = lv_obj_create(ui_ScreenCameraShot);
lv_obj_set_height( ui_PanelCameraShotTitle, 80);
lv_obj_set_width( ui_PanelCameraShotTitle, lv_pct(100));
lv_obj_set_align( ui_PanelCameraShotTitle, LV_ALIGN_TOP_MID );
lv_obj_clear_flag( ui_PanelCameraShotTitle, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_PanelCameraShotTitle, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_PanelCameraShotTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_PanelCameraShotTitle, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_PanelCameraShotTitle, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_LabelCameraShotTitle = lv_label_create(ui_PanelCameraShotTitle);
lv_obj_set_width( ui_LabelCameraShotTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_LabelCameraShotTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_LabelCameraShotTitle, LV_ALIGN_CENTER );
lv_label_set_text(ui_LabelCameraShotTitle,"Camera");
lv_obj_set_style_text_color(ui_LabelCameraShotTitle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_LabelCameraShotTitle, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
// lv_obj_set_style_text_font(ui_LabelCameraShotTitle, &lv_font_montserrat_38, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ImageCameraShotImage = lv_canvas_create(ui_ScreenCameraShot);
lv_obj_set_height( ui_ImageCameraShotImage, 720);
lv_obj_set_width( ui_ImageCameraShotImage, 1280);
lv_obj_set_align( ui_ImageCameraShotImage, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ImageCameraShotImage, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ImageCameraShotImage, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_PanelCameraShotControlBg = lv_obj_create(ui_ImageCameraShotImage);
lv_obj_set_width( ui_PanelCameraShotControlBg, 70);
lv_obj_set_height( ui_PanelCameraShotControlBg, 70);
lv_obj_set_align( ui_PanelCameraShotControlBg, LV_ALIGN_BOTTOM_MID );
lv_obj_set_y( ui_PanelCameraShotControlBg, -190 );
lv_obj_clear_flag( ui_PanelCameraShotControlBg, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_PanelCameraShotControlBg, 256, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_PanelCameraShotControlBg, lv_color_hex(0x929292), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_PanelCameraShotControlBg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_PanelCameraShotControlBg, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ButtonCameraShotBtn = lv_btn_create(ui_ImageCameraShotImage);
lv_obj_set_width( ui_ButtonCameraShotBtn, 50);
lv_obj_set_height( ui_ButtonCameraShotBtn, 50);
lv_obj_set_align( ui_ButtonCameraShotBtn, LV_ALIGN_BOTTOM_MID );
lv_obj_set_y( ui_ButtonCameraShotBtn, -200 );
lv_obj_add_flag( ui_ButtonCameraShotBtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_ButtonCameraShotBtn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_ButtonCameraShotBtn, 256, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_ButtonCameraShotBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ButtonCameraShotBtn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_PanelCameraShotAlbum = lv_obj_create(ui_ImageCameraShotImage);
lv_obj_set_width( ui_PanelCameraShotAlbum, 80);
lv_obj_set_height( ui_PanelCameraShotAlbum, 80);
lv_obj_set_x( ui_PanelCameraShotAlbum, -450 );
lv_obj_set_y( ui_PanelCameraShotAlbum, -170 );
lv_obj_set_align( ui_PanelCameraShotAlbum, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_PanelCameraShotAlbum, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_PanelCameraShotAlbum, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_PanelCameraShotAlbum, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_clip_corner(ui_PanelCameraShotAlbum, true, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_PanelCameraShotAlbum, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_PanelCameraShotAlbum, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_PanelCameraShotAlbum, ui_event_PanelCameraShotAlbum, LV_EVENT_ALL, NULL);

}
