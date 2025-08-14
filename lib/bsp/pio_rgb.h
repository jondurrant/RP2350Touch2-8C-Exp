#ifndef __PIO_RGB_H__
#define __PIO_RGB_H__

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "bsp_dma_channel_irq.h"

typedef struct  {

// public
    uint16_t width;
    uint16_t height;
    uint16_t *framebuffer1;
    uint16_t *framebuffer2;
    uint16_t *transfer_buffer1;
    uint16_t *transfer_buffer2;
    size_t transfer_size;
    size_t pclk_freq;
    channel_irq_callback_t dma_flush_done_cb;
    struct {
        bool double_buffer;
        bool enabled_transfer;
        bool enabled_psram;
    } mode;
// private
    uint16_t *_framebuffer;
    bool change_framebuffer_flag;
    uint16_t transfer_index;
    uint16_t transfer_index_max;

} pio_rgb_info_t;


typedef struct 
{
    uint hsync_pin;
    uint vsync_pin;
    uint plck_pin;
    uint de_pin;
    uint data0_pin;
} pio_rgb_pin_t;



#ifdef __cplusplus
extern "C" {
#endif 

void pio_rgb_init(pio_rgb_info_t *info, pio_rgb_pin_t *pin);

/**
 * @brief 切换帧缓冲区
 */
void pio_rgb_change_framebuffer(void);

/**
 * @brief 获取空闲的帧缓冲区
 */

 uint16_t *pio_rgb_get_free_framebuffer(void);


/**
 * @brief 更新帧缓冲区的内容
 */
void pio_rgb_update_framebuffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *color_p);


#ifdef __cplusplus
}
#endif

#endif