// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.3
// LVGL version: 8.3.6
// Project name: Camera

#include "ui.h"

///////////////////// VARIABLES ////////////////////


// SCREEN: ui_ScreenCameraShot
void ui_ScreenCameraShot_screen_init(void);
lv_obj_t *ui_ScreenCameraShot;
lv_obj_t *ui_PanelCameraShotTitle;
lv_obj_t *ui_LabelCameraShotTitle;
lv_obj_t *ui_ImageCameraShotImage;
lv_obj_t *ui_PanelCameraShotControl;
lv_obj_t *ui_PanelCameraShotControlBg;
lv_obj_t *ui_ButtonCameraShotBtn;
void ui_event_PanelCameraShotAlbum( lv_event_t * e);
lv_obj_t *ui_PanelCameraShotAlbum;


// SCREEN: ui_ScreenCameraPhoto
void ui_ScreenCameraPhoto_screen_init(void);
void ui_event_ScreenCameraPhoto( lv_event_t * e);
lv_obj_t *ui_ScreenCameraPhoto;
lv_obj_t *ui_PanelCameraPhotoTitle;
lv_obj_t *ui_LabelCameraPhotoTitle;
lv_obj_t *ui_ImageCameraPhotoImage;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_PanelCameraShotAlbum( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( &ui_ScreenCameraPhoto, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCameraPhoto_screen_init);
}
}
void ui_event_ScreenCameraPhoto( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( &ui_ScreenCameraShot, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCameraShot_screen_init);
}
}

///////////////////// SCREENS ////////////////////

void ui_camera_init( void )
{
ui_ScreenCameraShot_screen_init();
ui_ScreenCameraPhoto_screen_init();
lv_disp_load_scr( ui_ScreenCameraShot);
}
