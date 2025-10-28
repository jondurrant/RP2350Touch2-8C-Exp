/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "pio_rgb.h"
#include "bsp_st7701s.h"

#define MY_DISP_HOR_RES (480)
#define MY_DISP_VER_RES (480)

bsp_display_interface_t *display_if;

static lv_color_t *buf_1;
static lv_color_t *buf_2;

static lv_area_t last_area;

static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/

static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    bsp_display_interface_t *display_if = (bsp_display_interface_t *)drv->user_data;
    bsp_display_area_t display_area = {
        .x1 = area->x1,
        .x2 = area->x2,
        .y1 = area->y1,
        .y2 = area->y2
    };
    display_if->flush_dma(&display_area, (uint16_t *)color_p);
    lv_disp_flush_ready(drv);
}


//__attribute__((section(".psram")))  uint16_t framebuffer1[MY_DISP_HOR_RES][MY_DISP_VER_RES];


void lv_port_disp_init(void)
{

    pio_rgb_info_t rgb_info;
    rgb_info.width = MY_DISP_HOR_RES;
    rgb_info.height = MY_DISP_VER_RES;
    rgb_info.transfer_size = MY_DISP_HOR_RES * 40;
    rgb_info.pclk_freq = BSP_LCD_PCLK_FREQ;
    rgb_info.mode.double_buffer = false;
    rgb_info.mode.enabled_transfer = true;
    rgb_info.mode.enabled_psram = false;
    rgb_info.framebuffer1 = malloc(MY_DISP_HOR_RES * MY_DISP_VER_RES * sizeof(uint16_t));
    //rgb_info.framebuffer1 = (void *)framebuffer1;
    rgb_info.dma_flush_done_cb = NULL;

    bsp_display_info_t display_info;
    display_info.width = MY_DISP_HOR_RES;
    display_info.height = MY_DISP_VER_RES;
    display_info.brightness = 80;
    display_info.dma_flush_done_cb = NULL;
    display_info.user_data = &rgb_info;

    bsp_display_new_st7701s(&display_if, &display_info);
    display_if->init();

    static lv_disp_draw_buf_t draw_buf_dsc_1;
    buf_1 = malloc(MY_DISP_HOR_RES * 10 * sizeof(lv_color_t));
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);  
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    disp_drv.user_data = display_if;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}


#endif
