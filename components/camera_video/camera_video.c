/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/errno.h>
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "sdkconfig.h"
#include "linux/videodev2.h"
#include "esp_video_init.h"
#include "esp_video_device.h"
#include "driver/jpeg_encode.h"
#include "qmsd_board_pin.h"
#include "driver/i2c_master.h"
#include "zx4d30ce40s_lcd_rgb.h"

// video frame buffer count, too large value may cause memory allocation fails.
#define EXAMPLE_VIDEO_BUFFER_COUNT 2
#define MEMORY_TYPE V4L2_MEMORY_MMAP
#define CAM_DEV_PATH ESP_VIDEO_MIPI_CSI_DEVICE_NAME
#define JPEG_ENC_QUALITY (80)
#define PART_BOUNDARY "123456789000000000000987654321"
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif
#define EXAMPLE_MDNS_INSTANCE "simple video web"
#define EXAMPLE_MDNS_HOST_NAME "esp-web"

int video_cam_fd;
static uint8_t g_video_status;

/*
 * Web cam control structure
 */
typedef struct web_cam
{
    int fd;
    uint32_t width;
    uint32_t height;
    uint32_t pixel_format;
    jpeg_encode_cfg_t jpeg_enc_config;
    size_t jpeg_enc_output_buf_alloced_size;
    jpeg_encoder_handle_t jpeg_handle;
    uint8_t *jpeg_out_buf;
    uint8_t *buffer[EXAMPLE_VIDEO_BUFFER_COUNT];
} web_cam_t;

/*
 * The image format type definition used in the example.
 */
typedef enum
{
    EXAMPLE_VIDEO_FMT_RAW8 = V4L2_PIX_FMT_SBGGR8,
    EXAMPLE_VIDEO_FMT_RAW10 = V4L2_PIX_FMT_SBGGR10,
    EXAMPLE_VIDEO_FMT_GREY = V4L2_PIX_FMT_GREY,
    EXAMPLE_VIDEO_FMT_RGB565 = V4L2_PIX_FMT_RGB565,
    EXAMPLE_VIDEO_FMT_RGB888 = V4L2_PIX_FMT_RGB24,
    EXAMPLE_VIDEO_FMT_YUV422 = V4L2_PIX_FMT_YUV422P,
    EXAMPLE_VIDEO_FMT_YUV420 = V4L2_PIX_FMT_YUV420,
} example_fmt_t;

static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";
const int s_queue_buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
static const char *TAG = "example";

/**
 * @brief   Open the video device and initialize the video device to use `init_fmt` as the output format.
 * @note    When the sensor outputs data in RAW format, the ISP module can interpolate its data into RGB or YUV format.
 *          However, when the sensor works in RGB or YUV format, the output data can only be in RGB or YUV format.
 * @param dev device name(eg, "/dev/video0")
 * @param init_fmt output format.
 *
 * @return
 *     - Device descriptor   Success
 *     - -1 error
 */
static int app_video_open(char *dev, example_fmt_t init_fmt)
{
    struct v4l2_format default_format;
    struct v4l2_capability capability;
    const int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int fd = open(dev, O_RDONLY);
    if (fd < 0)
    {
        ESP_LOGE(TAG, "Open video failed");
        return -1;
    }

    if (ioctl(fd, VIDIOC_QUERYCAP, &capability))
    {
        ESP_LOGE(TAG, "failed to get capability");
        goto exit_0;
    }

    ESP_LOGI(TAG, "version: %d.%d.%d", (uint16_t)(capability.version >> 16), (uint8_t)(capability.version >> 8), (uint8_t)capability.version);
    ESP_LOGI(TAG, "driver:  %s", capability.driver);
    ESP_LOGI(TAG, "card:    %s", capability.card);
    ESP_LOGI(TAG, "bus:     %s", capability.bus_info);

    memset(&default_format, 0, sizeof(struct v4l2_format));
    default_format.type = type;
    if (ioctl(fd, VIDIOC_G_FMT, &default_format) != 0)
    {
        ESP_LOGE(TAG, "failed to get format");
        goto exit_0;
    }

    ESP_LOGI(TAG, "width=%" PRIu32 " height=%" PRIu32, default_format.fmt.pix.width, default_format.fmt.pix.height);

    if (default_format.fmt.pix.pixelformat != init_fmt)
    {
        struct v4l2_format format = {
            .type = type,
            .fmt.pix.width = default_format.fmt.pix.width,
            .fmt.pix.height = default_format.fmt.pix.height,
            .fmt.pix.pixelformat = init_fmt,
        };

        if (ioctl(fd, VIDIOC_S_FMT, &format) != 0)
        {
            ESP_LOGE(TAG, "failed to set format");
            goto exit_0;
        }
    }

    return fd;
exit_0:
    close(fd);
    return -1;
}

static jpeg_enc_input_format_t get_jpeg_enc_input_fmt(uint32_t video_fmt)
{
    jpeg_enc_input_format_t ret_fmt = JPEG_ENCODE_IN_FORMAT_YUV422;
    switch (video_fmt)
    {
    case EXAMPLE_VIDEO_FMT_YUV422:
        ret_fmt = JPEG_ENCODE_IN_FORMAT_YUV422;
        break;
    case EXAMPLE_VIDEO_FMT_RAW8: // Treat raw8 as grayscale, for testing only.
    case EXAMPLE_VIDEO_FMT_GREY:
        ret_fmt = JPEG_ENCODE_IN_FORMAT_GRAY;
        break;
    case EXAMPLE_VIDEO_FMT_RGB565:
        ret_fmt = JPEG_ENCODE_IN_FORMAT_RGB565;
        break;
    case EXAMPLE_VIDEO_FMT_RGB888:
        ret_fmt = JPEG_ENCODE_IN_FORMAT_RGB888;
        break;
    default:
        ESP_LOGE(TAG, "Unsupported format");
        ret_fmt = -1;
        break;
    }
    return ret_fmt;
}

static esp_err_t record_bin_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_FAIL;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_buffer buf;
    web_cam_t *wc = (web_cam_t *)req->user_ctx;

    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=record.bin"); // default name is record.bin
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    memset(&buf, 0, sizeof(buf));
    buf.type = type;
    buf.memory = MEMORY_TYPE;
    res = ioctl(wc->fd, VIDIOC_DQBUF, &buf);
    if (res == 0)
    {
        res = httpd_resp_send_chunk(req, (const char *)wc->buffer[buf.index], buf.bytesused);
        if (res != ESP_OK)
        {
            ESP_LOGW(TAG, "chunk send failed");
        }
    }
    else
    {
        ESP_LOGE(TAG, "failed to receive video frame");
        return ESP_FAIL;
    }

    if (ioctl(wc->fd, VIDIOC_QBUF, &buf) != 0)
    {
        ESP_LOGE(TAG, "failed to free video frame");
    }

    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return res;
}

typedef struct
{
    uint8_t *jpeg_buf;
    uint32_t jpeg_size;
} jpeg_buf_t;

QueueHandle_t jpeg_queue;

static void debug_task(void *arg)
{
    struct v4l2_buffer buf;
    esp_err_t res = ESP_FAIL;
    web_cam_t *wc = (web_cam_t *)arg;

    while (1)
    {
        struct timespec ts = {0};
        memset(&buf, 0, sizeof(buf));
        buf.type = s_queue_buf_type;
        buf.memory = MEMORY_TYPE;

        res = ioctl(wc->fd, VIDIOC_DQBUF, &buf);
        if (res != 0)
        {
            ESP_LOGE(TAG, "failed to receive video frame");
            break;
        }

        res = clock_gettime(CLOCK_MONOTONIC, &ts);
        if (res != 0)
        {
            ESP_LOGE(TAG, "failed to get time");
        }

        uint32_t jpeg_encoded_size = 0;

        res = jpeg_encoder_process(wc->jpeg_handle, &wc->jpeg_enc_config, wc->buffer[buf.index], buf.bytesused, wc->jpeg_out_buf, wc->jpeg_enc_output_buf_alloced_size,
                                   &jpeg_encoded_size);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "jpeg size = %"PRIu32, jpeg_encoded_size);
        }
        uint8_t *jpeg = (uint8_t *)heap_caps_malloc(jpeg_encoded_size, MALLOC_CAP_SPIRAM);
        jpeg_buf_t jpeg_buf = {
            .jpeg_buf = jpeg,
            .jpeg_size = jpeg_encoded_size,
        };
        lv_memcpy(jpeg, wc->jpeg_out_buf, jpeg_encoded_size);
        if (xQueueSend(jpeg_queue, &jpeg_buf, pdMS_TO_TICKS(1000)) == pdFALSE)
        {
            free(jpeg);
        }

        if (ioctl(wc->fd, VIDIOC_QBUF, &buf) != 0)
        {
            ESP_LOGE(TAG, "failed to free video frame");
        }
    }
}

/* Note that opening this stream will block the use of other handlers.
You can access other handlers normally only after closing the stream.*/
static esp_err_t stream_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_FAIL;
    struct v4l2_buffer buf;
    uint8_t *jpeg_ptr = NULL;
    size_t jpeg_size = 0;
    bool tx_valid = false;
    uint32_t jpeg_encoded_size = 0;
    web_cam_t *wc = (web_cam_t *)req->user_ctx;

    ESP_ERROR_CHECK(httpd_resp_set_type(req, STREAM_CONTENT_TYPE));

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "5");

    while (1)
    {
        jpeg_buf_t buf;
        xQueueReceive(jpeg_queue, &buf, portMAX_DELAY);
        tx_valid = 1;

        struct timespec ts = {0};

        res = clock_gettime(CLOCK_MONOTONIC, &ts);
        if (res != 0)
        {
            ESP_LOGE(TAG, "failed to get time");
        }

        if (httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY)) != ESP_OK)
        {
            free(buf.jpeg_buf);
            break;
        }

        if (tx_valid)
        {
            int hlen;
            char part_buf[128];

            hlen = snprintf(part_buf, sizeof(part_buf), STREAM_PART, buf.jpeg_size, ts.tv_sec, ts.tv_nsec);
            res = httpd_resp_send_chunk(req, part_buf, hlen);
            if (res == ESP_OK)
            {
                res = httpd_resp_send_chunk(req, (char *)buf.jpeg_buf, buf.jpeg_size);
            }
        }
        free(buf.jpeg_buf);
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "Break stream handler");
            break;
        }
    }

    return res;
}

static esp_err_t pic_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_FAIL;
    struct v4l2_buffer buf;
    uint8_t *jpeg_ptr = NULL;
    size_t jpeg_size = 0;
    bool tx_valid = false;
    uint32_t jpeg_encoded_size = 0;
    web_cam_t *wc = (web_cam_t *)req->user_ctx;

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    memset(&buf, 0, sizeof(buf));
    buf.type = s_queue_buf_type;
    buf.memory = MEMORY_TYPE;
    res = ioctl(wc->fd, VIDIOC_DQBUF, &buf);
    if (res == 0)
    {
        if (wc->pixel_format == V4L2_PIX_FMT_JPEG)
        {
            jpeg_ptr = wc->buffer[buf.index];
            jpeg_size = buf.bytesused;
            tx_valid = true;
        }
        else
        {
            res = jpeg_encoder_process(wc->jpeg_handle, &wc->jpeg_enc_config, wc->buffer[buf.index], buf.bytesused, wc->jpeg_out_buf, wc->jpeg_enc_output_buf_alloced_size,
                                       &jpeg_encoded_size);
            if (res == ESP_OK)
            {
                jpeg_ptr = wc->jpeg_out_buf;
                jpeg_size = jpeg_encoded_size;
                tx_valid = true;
                ESP_LOGI(TAG, "jpeg size = %d", jpeg_size);
            }
            else
            {
                ESP_LOGE(TAG, "jpeg encode failed");
            }
        }

        if (tx_valid)
        {
            res = httpd_resp_send_chunk(req, (const char *)jpeg_ptr, jpeg_size);
            if (res != ESP_OK)
            {
                ESP_LOGE(TAG, "send chunk failed");
            }
        }

        if (ioctl(wc->fd, VIDIOC_QBUF, &buf) != 0)
        {
            ESP_LOGE(TAG, "failed to free video frame");
        }

        /* Respond with an empty chunk to signal HTTP response completion */
        httpd_resp_send_chunk(req, NULL, 0);
    }
    else
    {
        ESP_LOGE(TAG, "failed to receive video frame");
    }

    return res;
}

static esp_err_t new_web_cam(int cam_fd, web_cam_t **ret_wc)
{
    int ret;
    struct v4l2_format format;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_requestbuffers req;
    web_cam_t *wc;
    size_t jpeg_enc_input_src_size;

    memset(&format, 0, sizeof(struct v4l2_format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam_fd, VIDIOC_G_FMT, &format) != 0)
    {
        ESP_LOGE(TAG, "Failed get fmt");
        return ESP_FAIL;
    }

    wc = malloc(sizeof(web_cam_t));
    if (!wc)
    {
        return ESP_ERR_NO_MEM;
    }

    wc->fd = cam_fd;
    wc->width = format.fmt.pix.width;
    wc->height = format.fmt.pix.height;
    wc->pixel_format = format.fmt.pix.pixelformat;

    jpeg_enc_input_format_t jpeg_enc_infmt = get_jpeg_enc_input_fmt(format.fmt.pix.pixelformat);

    wc->jpeg_enc_config.src_type = jpeg_enc_infmt;
    wc->jpeg_enc_config.image_quality = JPEG_ENC_QUALITY;
    wc->jpeg_enc_config.width = format.fmt.pix.width;
    wc->jpeg_enc_config.height = format.fmt.pix.height;

    if (wc->pixel_format == EXAMPLE_VIDEO_FMT_RAW8)
    {
        wc->jpeg_enc_config.sub_sample = JPEG_DOWN_SAMPLING_GRAY;
        jpeg_enc_input_src_size = format.fmt.pix.width * format.fmt.pix.height;
    }
    else if (wc->pixel_format == EXAMPLE_VIDEO_FMT_GREY)
    {
        wc->jpeg_enc_config.sub_sample = JPEG_DOWN_SAMPLING_GRAY;
        jpeg_enc_input_src_size = format.fmt.pix.width * format.fmt.pix.height;
    }
    else if (wc->pixel_format == EXAMPLE_VIDEO_FMT_YUV420)
    {
        wc->jpeg_enc_config.sub_sample = JPEG_DOWN_SAMPLING_YUV420;
        jpeg_enc_input_src_size = format.fmt.pix.width * format.fmt.pix.height * 3 / 2;
    }
    else
    {
        wc->jpeg_enc_config.sub_sample = JPEG_DOWN_SAMPLING_YUV422;
        jpeg_enc_input_src_size = format.fmt.pix.width * format.fmt.pix.height * 2;
    }

    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .timeout_ms = 5000,
    };
    ESP_ERROR_CHECK(jpeg_new_encoder_engine(&encode_eng_cfg, &wc->jpeg_handle));

    jpeg_encode_memory_alloc_cfg_t jpeg_enc_output_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    // Note that a larger JPEG_ENC_QUALITY means better image quality, so you need to increase the allocated buffer size
    wc->jpeg_out_buf = (uint8_t *)jpeg_alloc_encoder_mem(100 * 1024, &jpeg_enc_output_mem_cfg, &wc->jpeg_enc_output_buf_alloced_size);
    if (!wc->jpeg_out_buf)
    {
        ESP_LOGE(TAG, "failed to alloc jpeg output buf");
        ret = ESP_ERR_NO_MEM;
        goto errout;
    }

    memset(&req, 0, sizeof(req));
    req.count = ARRAY_SIZE(wc->buffer);
    req.type = type;
    req.memory = MEMORY_TYPE;
    if (ioctl(wc->fd, VIDIOC_REQBUFS, &req) != 0)
    {
        ESP_LOGE(TAG, "failed to req buffers");
        ret = ESP_FAIL;
        goto errout;
    }

    for (int i = 0; i < ARRAY_SIZE(wc->buffer); i++)
    {
        struct v4l2_buffer buf;

        memset(&buf, 0, sizeof(buf));
        buf.type = type;
        buf.memory = MEMORY_TYPE;
        buf.index = i;
        if (ioctl(wc->fd, VIDIOC_QUERYBUF, &buf) != 0)
        {
            ESP_LOGE(TAG, "failed to query buffer");
            ret = ESP_FAIL;
            goto errout;
        }

        wc->buffer[i] = (uint8_t *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, wc->fd, buf.m.offset);
        if (!wc->buffer[i])
        {
            ESP_LOGE(TAG, "failed to map buffer");
            ret = ESP_FAIL;
            goto errout;
        }

        if (ioctl(wc->fd, VIDIOC_QBUF, &buf) != 0)
        {
            ESP_LOGE(TAG, "failed to queue frame buffer");
            ret = ESP_FAIL;
            goto errout;
        }
    }

    if (ioctl(wc->fd, VIDIOC_STREAMON, &type))
    {
        ESP_LOGE(TAG, "failed to start stream");
        ret = ESP_FAIL;
        goto errout;
    }

    *ret_wc = wc;
    return ESP_OK;

errout:
    free(wc);
    return ret;
}

static esp_err_t http_server_init(int index, web_cam_t *web_cam)
{
    esp_err_t ret;
    httpd_handle_t video_web_httpd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 1024 * 8;
    config.server_port += index;
    config.ctrl_port += index;
    config.core_id = 0;
    httpd_uri_t pic_get_uri = {.uri = "/pic", .method = HTTP_GET, .handler = pic_handler, .user_ctx = (void *)web_cam};
    httpd_uri_t record_file_get_uri = {
        .uri = "/record",
        .method = HTTP_GET,
        .handler = record_bin_handler,
        .user_ctx = (void *)web_cam,
    };
    httpd_uri_t stream_get_uri = {.uri = "/stream", .method = HTTP_GET, .handler = stream_handler, .user_ctx = (void *)web_cam};

    ret = httpd_start(&video_web_httpd, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ret;
    }

    ESP_ERROR_CHECK(httpd_register_uri_handler(video_web_httpd, &pic_get_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(video_web_httpd, &record_file_get_uri));
    ESP_ERROR_CHECK((httpd_register_uri_handler(video_web_httpd, &stream_get_uri)));

    ESP_LOGI(TAG, "Starting stream HTTP server on port: '%d'", config.server_port);

    return ESP_OK;
}

/**
 * @brief   Build a web server with `cam_fd` as the image data source.
 * @param index The index number of the web server.
 * It is allowed to establish multiple servers, and its data port and control port are the default port + index
 * @param cam_fd Cam device descriptor.
 *
 * @return
 *     - ESP_OK   Success
 *     - Others error
 */
static esp_err_t start_cam_web_server(int index, int cam_fd)
{
    web_cam_t *web_cam;
    esp_err_t ret = new_web_cam(cam_fd, &web_cam);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to new web cam");
        return ret;
    }

    jpeg_queue = xQueueCreate(2, sizeof(jpeg_buf_t));
    xTaskCreatePinnedToCore(debug_task, "debug_task", 1024 * 8, web_cam, 5, NULL, 1);
    return http_server_init(index, web_cam);
}

void video_test_main(i2c_master_bus_handle_t i2c_handle)
{
    int index = 0;
    static esp_video_init_csi_config_t csi_config = {
        .sccb_config =
            {
                .init_sccb = false,
                .freq = 100 * 1000,
            },
        .reset_pin = -1,
        .pwdn_pin = -1,
    };

    if (i2c_handle != NULL)
    {
        csi_config.sccb_config.i2c_handle = i2c_handle;
    }

    static esp_video_init_config_t cam_config = {
        .csi = &csi_config,
    };
    ESP_ERROR_CHECK(esp_video_init(&cam_config));

    video_cam_fd = app_video_open(CAM_DEV_PATH, EXAMPLE_VIDEO_FMT_RGB565);
    if (video_cam_fd < 0)
    {
        g_video_status = 2;
        ESP_LOGE(TAG, "video cam open failed");
        return;
    }

    g_video_status = 1;
    ESP_ERROR_CHECK(start_cam_web_server(index, video_cam_fd));
    ESP_LOGI(TAG, "Example Start");
}

uint8_t camera_video_get_status()
{
    return g_video_status;
}