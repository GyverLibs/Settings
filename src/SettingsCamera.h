#pragma once
#include <esp_camera.h>
#include <esp_http_server.h>
#include <esp_wifi.h>
#include <inttypes.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>

namespace sets {

static bool cameraInit(framesize_t frame_size = FRAMESIZE_VGA, pixformat_t pixel_format = PIXFORMAT_JPEG, int jpeg_quality = 12);
static void streamBegin(uint16_t fps = 30, uint16_t port = 82, const char* path = "/stream");
static void streamEnd();

// ================= CAMERA =================
#define CAMERA_MODEL_AI_THINKER
// #define CAMERA_MODEL_WROVER_KIT
// #define CAMERA_MODEL_M5STACK_PSRAM
// #define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM

#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 21
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 19
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 5
#define Y2_GPIO_NUM 4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM 15
#define XCLK_GPIO_NUM 27
#define SIOD_GPIO_NUM 25
#define SIOC_GPIO_NUM 23

#define Y9_GPIO_NUM 19
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 39
#define Y5_GPIO_NUM 5
#define Y4_GPIO_NUM 34
#define Y3_GPIO_NUM 35
#define Y2_GPIO_NUM 32
#define VSYNC_GPIO_NUM 22
#define HREF_GPIO_NUM 26
#define PCLK_GPIO_NUM 21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM 15
#define XCLK_GPIO_NUM 27
#define SIOD_GPIO_NUM 25
#define SIOC_GPIO_NUM 23

#define Y9_GPIO_NUM 19
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 39
#define Y5_GPIO_NUM 5
#define Y4_GPIO_NUM 34
#define Y3_GPIO_NUM 35
#define Y2_GPIO_NUM 17
#define VSYNC_GPIO_NUM 22
#define HREF_GPIO_NUM 26
#define PCLK_GPIO_NUM 21

#elif defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#else
#error "Camera model not selected"
#endif

/*
    frame_size
    FRAMESIZE_96X96,    // 96x96
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_HVGA,     // 480x320
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_FHD,      // 1920x1080
    FRAMESIZE_P_HD,     // 720x1280
    FRAMESIZE_P_3MP,    // 864x1536
    FRAMESIZE_QXGA,     // 2048x1536
    FRAMESIZE_QHD,      // 2560x1440
    FRAMESIZE_WQXGA,    // 2560x1600
    FRAMESIZE_P_FHD,    // 1080x1920
    FRAMESIZE_QSXGA,    // 2560x1920
*/
/*
    pixel_format
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_YUV422,    // 2BPP/YUV422
    PIXFORMAT_YUV420,    // 1.5BPP/YUV420
    PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    PIXFORMAT_JPEG,      // JPEG/COMPRESSED
    PIXFORMAT_RGB888,    // 3BPP/RGB888
    PIXFORMAT_RAW,       // RAW
    PIXFORMAT_RGB444,    // 3BP2P/RGB444
    PIXFORMAT_RGB555,    // 3BP2P/RGB555
*/

// jpeg_quality - 0-63 lower means higher quality

static bool cameraInit(framesize_t frame_size, pixformat_t pixel_format, int jpeg_quality) {
    esp_wifi_set_ps(WIFI_PS_NONE);              // no power save
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = pixel_format;
    config.frame_size = frame_size;
    config.jpeg_quality = jpeg_quality;
    config.fb_count = 1;

    esp_err_t result = esp_camera_init(&config);
    return (result == ESP_OK);
}

// ================= STREAM =================
#define _SS_BOUND "-------stream"
static const char* _SS_TYPE = "multipart/x-mixed-replace;boundary=" _SS_BOUND;
static const char* _SS_HEADER = "Content-Type: image/jpeg\r\nContent-Length: ";
static const char* _SS_ENDL = "\r\n\r\n";
static const char* _SS_CHUNK_END = "\r\n--" _SS_BOUND "\r\n";

static httpd_handle_t _stream_httpd = NULL;
static bool _stream_state = false;
static uint16_t _frame_delay;

static esp_err_t _stream_handler(httpd_req_t* req) {
    esp_err_t res = httpd_resp_set_type(req, _SS_TYPE);
    camera_fb_t* fb = NULL;
    char buf[strlen(_SS_HEADER) + strlen(_SS_ENDL) + 7 + 1];

    while (1) {
        if (res != ESP_OK || !_stream_state) break;

        fb = esp_camera_fb_get();
        if (!fb) continue;

        strcpy(buf, _SS_HEADER);
        utoa(fb->len, buf + strlen(_SS_HEADER), 10);
        strcat(buf, _SS_ENDL);

        res = httpd_resp_send_chunk(req, (const char*)buf, strlen(buf));

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _SS_CHUNK_END, strlen(_SS_CHUNK_END));
        }
        esp_camera_fb_return(fb);

        delay(_frame_delay);
    }
    return res;
}

static void streamBegin(uint16_t fps, uint16_t port, const char* path) {
    _frame_delay = 1000 / fps;
    httpd_uri_t index = {path, HTTP_GET, _stream_handler, NULL};

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = port;
    cfg.send_wait_timeout = 1;
    cfg.max_uri_handlers = 8;

    if (httpd_start(&_stream_httpd, &cfg) == ESP_OK) {
        httpd_register_uri_handler(_stream_httpd, &index);
        _stream_state = 1;
    }
}

static void streamEnd() {
    if (_stream_httpd) httpd_stop(_stream_httpd);
    _stream_httpd = NULL;
    _stream_state = 0;
}

}  // namespace sets