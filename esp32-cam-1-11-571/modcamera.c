// Code adapted from https://github.com/espressif/esp32-camera

#include "esp_camera.h"
#include "esp_log.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"

#define TAG "camera"

//WROVER-KIT PIN Map
#define CAM_PIN_PWDN    32 //power down is not used
#define CAM_PIN_RESET   -1 //software reset will be performed
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26 // SDA
#define CAM_PIN_SIOC    27 // SCL

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

static camera_config_t camera_config = {
    .pin_pwdn  = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_XGA,//QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 12, //10-63 lower number means higher quality
    .fb_count = 1 //if more than one, i2s runs in continuous mode. Use only with JPEG
};

#include "esp_system.h"
#include "esp_spi_flash.h"


STATIC mp_obj_t camera_init(){
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return mp_const_false;
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(camera_init_obj, camera_init);


STATIC mp_obj_t camera_deinit(){
    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera deinit Failed");
        return mp_const_false;
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(camera_deinit_obj, camera_deinit);


STATIC mp_obj_t camera_capture(){
    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return mp_const_false;
    }

    mp_obj_t image = mp_obj_new_bytes(fb->buf, fb->len);

    //return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    return image;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(camera_capture_obj, camera_capture);

STATIC mp_obj_t camera_flip(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Flipping Failed");
        return mp_const_false;
      }
    int direction = mp_obj_get_int(what);
    s->set_vflip(s, direction);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_flip_obj, camera_flip);

STATIC mp_obj_t camera_pixformat(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Pixformat Failed");
        return mp_const_false;
      }
    int size = mp_obj_get_int(what);
    if (size == 0) {
      s->set_pixformat(s, PIXFORMAT_JPEG); // JPEG (default) compress
    } else if (size == 1) {
      s->set_pixformat(s, PIXFORMAT_GRAYSCALE); // Grayscale 1 byte/pixel
    } else if (size == 2) {
      s->set_pixformat(s, PIXFORMAT_RGB565); // Red Green Blue 3 bytes/pixcel
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_pixformat_obj, camera_pixformat);

STATIC mp_obj_t camera_framesize(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Framesize Failed");
        return mp_const_false;
      }
    int size = mp_obj_get_int(what);
    if (size == 1) {
      s->set_framesize(s, FRAMESIZE_QQVGA); // 160x120
    } else if (size == 2) {
      s->set_framesize(s, FRAMESIZE_QQVGA2); // 128x160
    } else if (size == 3) {
      s->set_framesize(s, FRAMESIZE_QCIF); // 176x144 
    } else if (size == 4) {
      s->set_framesize(s, FRAMESIZE_HQVGA); // 240x176
    } else if (size == 5) {
      s->set_framesize(s, FRAMESIZE_QVGA); // 320x240
    } else if (size == 6) {
      s->set_framesize(s, FRAMESIZE_CIF); // 400x296
    } else if (size == 7) {
      s->set_framesize(s, FRAMESIZE_VGA); // 640x480
    } else if (size == 8) {
      s->set_framesize(s, FRAMESIZE_SVGA); // 800x600
    } else if (size == 9) {
      s->set_framesize(s, FRAMESIZE_XGA); // 1024x768  (default)
    } else if (size == 10) {
      s->set_framesize(s, FRAMESIZE_SXGA); // 1280x1024
    } else if (size == 11) {
      s->set_framesize(s, FRAMESIZE_UXGA); // 1600x1200
    } else if (size == 12) {
      s->set_framesize(s, FRAMESIZE_QXGA); // 2048x1536
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_framesize_obj, camera_framesize);

STATIC mp_obj_t camera_quality(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Quality Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what); // 10-63 lower number means higher quality
    s->set_quality(s, val);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_quality_obj, camera_quality);

STATIC mp_obj_t camera_contrast(mp_obj_t what){
    //acquire a frame
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Contrast Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what); // -2,2 (default 0). 2 highcontrast
    s->set_contrast(s, val);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_contrast_obj, camera_contrast);

STATIC mp_obj_t camera_saturation(mp_obj_t what){
    //acquire a frame
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Saturation Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_saturation(s, val); // -2,2 (default 0). -2 grayscale
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_saturation_obj, camera_saturation);

STATIC mp_obj_t camera_brightness(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Brightness Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_brightness(s, val); // -2,2 (default 0). 2 brightest
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_brightness_obj, camera_brightness);

STATIC mp_obj_t camera_speffect(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Special Effect Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_special_effect(s, val); // 0-6 (default 0). 
                                   // 0 - no effect
				   // 1 - negative
				   // 2 - black and white
				   // 3 - reddish
				   // 4 - greenish
				   // 5 - blue
				   // 6 - retro
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_speffect_obj, camera_speffect);

STATIC mp_obj_t camera_whitebalance(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "White Balance Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_wb_mode(s, val); // 0-4 (default 0).
                                   // 0 - no effect
                                   // 1 - sunny
                                   // 2 - cloudy
                                   // 3 - office
                                   // 4 - home
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_whitebalance_obj, camera_whitebalance);

STATIC mp_obj_t camera_aelevels(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "AE Levels Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_ae_level(s, val); // -2 to +2 (default 0).
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_aelevels_obj, camera_aelevels);

STATIC mp_obj_t camera_aecvalue(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "AEC Value Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_aec_value(s, val); // 0 to 1200 (default 0).
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_aecvalue_obj, camera_aecvalue);

STATIC mp_obj_t camera_agcgain(mp_obj_t what){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "AGC Gain Failed");
        return mp_const_false;
      }
    int val = mp_obj_get_int(what);
    s->set_agc_gain(s, val); // 0 to 30 (default 30).
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(camera_agcgain_obj, camera_agcgain);
STATIC mp_obj_t camera_info(){
    sensor_t * s = esp_camera_sensor_get();
    if (!s) {
        ESP_LOGE(TAG, "Info Failed. Run init.");
        return mp_const_false;
      }

    int py_pixformat;

    switch(s->pixformat) {
        case PIXFORMAT_JPEG:
            py_pixformat=0;
            break;
        case PIXFORMAT_GRAYSCALE:
            py_pixformat=1;
            break;
        case PIXFORMAT_RGB565:
            py_pixformat=2;
            break;
        default:
            ESP_LOGE(TAG, "Invalid pixformat returned from sensor.");
            py_pixformat=99;
            break;
    }

    ESP_LOGI(TAG, "aelevels:     %3u", s->status.ae_level);
    ESP_LOGI(TAG, "aecvalue:     %3u", s->status.aec);
    ESP_LOGI(TAG, "agc:          %3u", s->status.agc);
    ESP_LOGI(TAG, "brightness:   %3u", s->status.brightness);
    ESP_LOGI(TAG, "contrast:     %3u", s->status.contrast);
    ESP_LOGI(TAG, "framesize:    %3u", s->status.framesize);
    ESP_LOGI(TAG, "pixformat:    %3u", py_pixformat);
    ESP_LOGI(TAG, "quality:      %3u", s->status.quality);
    ESP_LOGI(TAG, "saturation:   %3u", s->status.saturation);
    ESP_LOGI(TAG, "speffect:     %3u", s->status.special_effect);
    ESP_LOGI(TAG, "flip:         %3u", s->status.vflip);
    ESP_LOGI(TAG, "whitebalance: %3u", s->status.wb_mode);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(camera_info_obj, camera_info);

STATIC const mp_rom_map_elem_t camera_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&camera_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&camera_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip), MP_ROM_PTR(&camera_flip_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixformat), MP_ROM_PTR(&camera_pixformat_obj) },
    { MP_ROM_QSTR(MP_QSTR_framesize), MP_ROM_PTR(&camera_framesize_obj) },
    { MP_ROM_QSTR(MP_QSTR_quality), MP_ROM_PTR(&camera_quality_obj) },
    { MP_ROM_QSTR(MP_QSTR_contrast), MP_ROM_PTR(&camera_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_saturation), MP_ROM_PTR(&camera_saturation_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&camera_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_speffect), MP_ROM_PTR(&camera_speffect_obj) },
    { MP_ROM_QSTR(MP_QSTR_whitebalance), MP_ROM_PTR(&camera_whitebalance_obj) },
    { MP_ROM_QSTR(MP_QSTR_aelevels), MP_ROM_PTR(&camera_aelevels_obj) },
    { MP_ROM_QSTR(MP_QSTR_aecvalue), MP_ROM_PTR(&camera_aecvalue_obj) },
    { MP_ROM_QSTR(MP_QSTR_agcgain), MP_ROM_PTR(&camera_agcgain_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&camera_info_obj) },
};

STATIC MP_DEFINE_CONST_DICT(camera_module_globals, camera_module_globals_table);

const mp_obj_module_t mp_module_camera = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&camera_module_globals,
};
