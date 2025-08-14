#ifndef __BSP_ST7701s_H__    
#define __BSP_ST7701s_H__
#include "bsp_display.h"

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "bsp_display.h"

#define BSP_LCD_DE_PIN 20
#define BSP_LCD_VSYNC_PIN 21
#define BSP_LCD_HSYNC_PIN 22
#define BSP_LCD_PLCK_PIN 23
#define BSP_LCD_DATA0_PIN 24
#define BSP_LCD_RST_PIN 16
// #define BSP_LCD_EN_PIN 18

#define BSP_LCD_CS_PIN 17
#define BSP_LCD_SCK_PIN 18
#define BSP_LCD_SDA_PIN 19


#define LCD_DATA0_PIN 24 // Blue - LSB


#define BSP_LCD_PCLK_FREQ   (16 * 1000 * 1000)


#define BSP_LCD_BL_PIN 9

#define PWM_FREQ 5000
#define PWM_WRAP 1000

typedef struct
{
    uint8_t reg;           /*<! The specific LCD command */
    uint8_t *data;         /*<! Buffer that holds the command specific data */
    size_t data_bytes;     /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms; /*<! Delay in milliseconds after this command */
} bsp_st7701s_cmd_t;



#ifdef __cplusplus
extern "C" {
#endif

bool bsp_display_new_st7701s(bsp_display_interface_t **interface, bsp_display_info_t *info);
#ifdef __cplusplus
}
#endif


#endif

