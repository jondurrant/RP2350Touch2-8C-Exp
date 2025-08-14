#ifndef __BSP_DISPLAY_H__
#define __BSP_DISPLAY_H__

#include "bsp_dma_channel_irq.h"

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t rotation;

    uint8_t brightness;
    uint dma_tx_channel;
    void *user_data;
    channel_irq_callback_t dma_flush_done_cb;
}bsp_display_info_t;

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} bsp_display_area_t;

typedef struct bsp_display_interface_t bsp_display_interface_t;

/**
 * @brief Structure for display interface
 */
struct bsp_display_interface_t {
   void (*init)(void);
   void (*reset)(void);

   void (*set_rotation)(uint16_t rotation);
   void (*set_brightness)(uint8_t brightness);

   void (*set_window)(bsp_display_area_t *area);
   
   void (*get_rotation)(uint16_t *rotation);
   void (*get_brightness)(uint8_t *brightness);

   void (*flush)(bsp_display_area_t *area, uint16_t *color_p);
   void (*flush_dma)(bsp_display_area_t *area, uint16_t *color_p);
};


#endif