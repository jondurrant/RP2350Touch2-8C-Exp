#include "bsp_st7701s.h"
#include "hardware/clocks.h"

#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "pio_rgb.h"

static bsp_display_interface_t *g_display_if;
static bsp_display_info_t *g_display_info;

static uint slice_num;
static uint pwm_channel;

/**
 * @brief LCD backlight brightness control
 */
static void bsp_lcd_brightness_init(void)
{
    float sys_clk = clock_get_hz(clk_sys);
    gpio_set_function(BSP_LCD_BL_PIN, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    slice_num = pwm_gpio_to_slice_num(BSP_LCD_BL_PIN);

    pwm_channel = pwm_gpio_to_channel(BSP_LCD_BL_PIN);

    pwm_set_clkdiv(slice_num, sys_clk / (PWM_FREQ * PWM_WRAP));

    pwm_set_wrap(slice_num, PWM_WRAP);

    pwm_set_chan_level(slice_num, pwm_channel, 0);

    pwm_set_enabled(slice_num, true);
}

/**
 * @brief Set the brightness of the LCD backlight
 */
static void bsp_lcd_set_brightness(uint8_t percent)
{
    if (percent > 100)
    {
        percent = 100;
    }
    pwm_set_chan_level(slice_num, pwm_channel, PWM_WRAP / 100 * percent);

    g_display_info->brightness = percent;
}

/**
 * @brief Reset the LCD
 */
static void bsp_st7701s_reset(void)
{
    gpio_put(BSP_LCD_RST_PIN, 1);
    sleep_ms(20);
    gpio_put(BSP_LCD_RST_PIN, 0);
    sleep_ms(20);
    gpio_put(BSP_LCD_RST_PIN, 1);
    sleep_ms(200);
}

bsp_st7701s_cmd_t init_cmds[] = {
    //  {cmd, { data }, data_size, delay_ms}

    {.reg = 0xFF, .data = (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x13}, .data_bytes = 5, .delay_ms = 0},
    {.reg = 0xEF, .data = (uint8_t[]){0x08}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xFF, .data = (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x10}, .data_bytes = 5, .delay_ms = 0},
    {.reg = 0xC0, .data = (uint8_t[]){0x3B, 0x00}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xC1, .data = (uint8_t[]){0x10, 0x0C}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xC2, .data = (uint8_t[]){0x07, 0x0A}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xC7, .data = (uint8_t[]){0x00}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xCC, .data = (uint8_t[]){0x10}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xCD, .data = (uint8_t[]){0x08}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB0, .data = (uint8_t[]){0x05, 0x12, 0x98, 0x0E, 0x0F, 0x07, 0x07, 0x09, 0x09, 0x23, 0x05, 0x52, 0x0F, 0x67, 0x2C, 0x11}, .data_bytes = 16, .delay_ms = 0},
    {.reg = 0xB1, .data = (uint8_t[]){0x0B, 0x11, 0x97, 0x0C, 0x12, 0x06, 0x06, 0x08, 0x08, 0x22, 0x03, 0x51, 0x11, 0x66, 0x2B, 0x0F}, .data_bytes = 16, .delay_ms = 0},
    {.reg = 0xFF, .data = (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x11}, .data_bytes = 5, .delay_ms = 0},
    {.reg = 0xB0, .data = (uint8_t[]){0x5D}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB1, .data = (uint8_t[]){0x3E}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB2, .data = (uint8_t[]){0x81}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB3, .data = (uint8_t[]){0x80}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB5, .data = (uint8_t[]){0x4E}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB7, .data = (uint8_t[]){0x85}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xB8, .data = (uint8_t[]){0x20}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xC1, .data = (uint8_t[]){0x78}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xC2, .data = (uint8_t[]){0x78}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xC3, .data = (uint8_t[]){0x01}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xD0, .data = (uint8_t[]){0x88}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0xE0, .data = (uint8_t[]){0x00, 0x00, 0x02}, .data_bytes = 3, .delay_ms = 0},
    {.reg = 0xE1, .data = (uint8_t[]){0x06, 0x30, 0x08, 0x30, 0x05, 0x30, 0x07, 0x30, 0x00, 0x33, 0x33}, .data_bytes = 11, .delay_ms = 0},
    {.reg = 0xE2, .data = (uint8_t[]){0x11, 0x11, 0x33, 0x33, 0xF4, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00, 0x00}, .data_bytes = 12, .delay_ms = 0},
    {.reg = 0xE3, .data = (uint8_t[]){0x00, 0x00, 0x11, 0x11}, .data_bytes = 4, .delay_ms = 0},
    {.reg = 0xE4, .data = (uint8_t[]){0x44, 0x44}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xE5, .data = (uint8_t[]){0x0D, 0xF5, 0x30, 0xF0, 0x0F, 0xF7, 0x30, 0xF0, 0x09, 0xF1, 0x30, 0xF0, 0x0B, 0xF3, 0x30, 0xF0}, .data_bytes = 16, .delay_ms = 0},
    {.reg = 0xE6, .data = (uint8_t[]){0x00, 0x00, 0x11, 0x11}, .data_bytes = 4, .delay_ms = 0},
    {.reg = 0xE7, .data = (uint8_t[]){0x44, 0x44}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xE8, .data = (uint8_t[]){0x0C, 0xF4, 0x30, 0xF0, 0x0E, 0xF6, 0x30, 0xF0, 0x08, 0xF0, 0x30, 0xF0, 0x0A, 0xF2, 0x30, 0xF0}, .data_bytes = 16, .delay_ms = 0},
    {.reg = 0xE9, .data = (uint8_t[]){0x36, 0x01}, .data_bytes = 2, .delay_ms = 0},
    {.reg = 0xEB, .data = (uint8_t[]){0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40}, .data_bytes = 7, .delay_ms = 0},
    {.reg = 0xED, .data = (uint8_t[]){0xFF, 0x10, 0xAF, 0x76, 0x54, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x45, 0x67, 0xFA, 0x01, 0xFF}, .data_bytes = 16, .delay_ms = 0},
    {.reg = 0xEF, .data = (uint8_t[]){0x08, 0x08, 0x08, 0x45, 0x3F, 0x54}, .data_bytes = 6, .delay_ms = 0},
    {.reg = 0xFF, .data = (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x00}, .data_bytes = 5, .delay_ms = 0},
    {.reg = 0x11, .data = NULL, .data_bytes = 0, .delay_ms = 120},
    {.reg = 0x3A, .data = (uint8_t[]){0x66}, .data_bytes = 1, .delay_ms = 10},
    {.reg = 0x36, .data = (uint8_t[]){0x00}, .data_bytes = 1, .delay_ms = 10},
    {.reg = 0x35, .data = (uint8_t[]){0x00}, .data_bytes = 1, .delay_ms = 0},
    {.reg = 0x29, .data = NULL, .data_bytes = 0, .delay_ms = 0},
    {.reg = 0x11, .data = NULL, .data_bytes = 0, .delay_ms = 120},
    // {.reg = 0x21, .data = NULL, .data_bytes = 0, .delay_ms = 0},
    {.reg = 0x20, .data = NULL, .data_bytes = 0, .delay_ms = 0},
};

void bsp_st7701s_spi_write(uint16_t data)
{
    for (int i = 8; i >= 0; i--)
    { // 从第 8 位到第 0 位
        // 设置 MOSI
        if (data & (1 << i))
        {
            gpio_put(BSP_LCD_SDA_PIN, 1);
        }
        else
        {
            gpio_put(BSP_LCD_SDA_PIN, 0);
        }

        // 产生 SCK 脉冲
        gpio_put(BSP_LCD_SCK_PIN, 1);
        sleep_us(100);
        gpio_put(BSP_LCD_SCK_PIN, 0);
        sleep_us(100);
    }
}

void bsp_st7701s_spi_write_reg(uint8_t reg)
{
    uint16_t buf = reg | 0x0000;
    bsp_st7701s_spi_write(buf);
}

void bsp_st7701s_spi_write_data(uint8_t data)
{
    uint16_t buf = (uint16_t)data | 0x0100;
    bsp_st7701s_spi_write(buf);
}

void bsp_st7701s_spi_tx_cmd(bsp_st7701s_cmd_t *cmds, size_t cmd_len)
{

    for (int i = 0; i < cmd_len; i++)
    {
        gpio_put(BSP_LCD_CS_PIN, 0);
        bsp_st7701s_spi_write_reg(cmds[i].reg);
        for (int j = 0; j < cmds[i].data_bytes; j++)
        {
            bsp_st7701s_spi_write_data(cmds[i].data[j]);
        }
        if (cmds[i].delay_ms > 0)
        {
            sleep_ms(cmds[i].delay_ms);
        }
        gpio_put(BSP_LCD_CS_PIN, 1);
    }
}

void bsp_st7701s_spi_init(void)
{
    gpio_init(BSP_LCD_CS_PIN);
    gpio_init(BSP_LCD_SCK_PIN);
    gpio_init(BSP_LCD_SDA_PIN);
    gpio_init(BSP_LCD_RST_PIN);

    gpio_set_dir(BSP_LCD_CS_PIN, GPIO_OUT);
    gpio_set_dir(BSP_LCD_SCK_PIN, GPIO_OUT);
    gpio_set_dir(BSP_LCD_SDA_PIN, GPIO_OUT);
    gpio_set_dir(BSP_LCD_RST_PIN, GPIO_OUT);
    gpio_put(BSP_LCD_CS_PIN, 1);

    bsp_st7701s_reset();

    bsp_st7701s_spi_tx_cmd(init_cmds, sizeof(init_cmds) / sizeof(bsp_st7701s_cmd_t));
}

/**
 * @brief Initialize the LCD
 */
static void bsp_st7701s_init(void)
{
    // Init pins

    // gpio_set_dir(BSP_LCD_EN_PIN, GPIO_OUT);
    // gpio_put(BSP_LCD_EN_PIN, 1);
    bsp_st7701s_spi_init();

    // Init brightness
    bsp_lcd_brightness_init();
    // Set brightness
    bsp_lcd_set_brightness(g_display_info->brightness);

    pio_rgb_pin_t pin;
    pio_rgb_info_t *rgb_info = (pio_rgb_info_t *)g_display_info->user_data;

    // printf("pio_rgb_init framebuffer1:0x%x\r\n", rgb_info->framebuffer1);
    // printf("pio_rgb_init framebuffer2:0x%x\r\n", rgb_info->framebuffer2);

    if (rgb_info->framebuffer1 == NULL)
    {
        printf("Error: Framebuffer1 is NULL\r\n");
        return;
    }
    for (size_t i = 0; i < rgb_info->width * rgb_info->height; i++)
    {
        rgb_info->framebuffer1[i] = 0xffff;
    }

    // double buffer mode
    if (rgb_info->mode.double_buffer)
    {
        if (rgb_info->framebuffer2 == NULL)
        {
            printf("Error: Framebuffer2 is NULL\r\n");
            return;
        }
    }

    // psram mode
    if (rgb_info->mode.enabled_psram)
    {
        if (rgb_info->transfer_buffer1 == NULL && rgb_info->transfer_buffer2 == NULL)
        {
            printf("Error: Transfer buffer1 or buffer2 is NULL\r\n");
            return;
        }
    }

    // rgb_info->pclk_freq = BSP_LCD_PCLK_FREQ;
    pin.data0_pin = BSP_LCD_DATA0_PIN;
    pin.de_pin = BSP_LCD_DE_PIN;
    pin.hsync_pin = BSP_LCD_HSYNC_PIN;
    pin.plck_pin = BSP_LCD_PLCK_PIN;
    pin.vsync_pin = BSP_LCD_VSYNC_PIN;

    pio_rgb_init(rgb_info, &pin);
}

void bsp_st7701s_flush_dma(bsp_display_area_t *area, uint16_t *color_p)
{
    if (area == NULL && color_p == NULL)
    {
        pio_rgb_change_framebuffer();
    }
    else
    {
        pio_rgb_update_framebuffer(area->x1, area->y1, area->x2, area->y2, color_p);
    }
}

bool bsp_display_new_st7701s(bsp_display_interface_t **interface, bsp_display_info_t *info)
{

    // check parameters
    if (info == NULL)
        return false;

    static bsp_display_interface_t display_if;
    static bsp_display_info_t display_info;

    memcpy(&display_info, info, sizeof(bsp_display_info_t));

    display_if.init = bsp_st7701s_init;
    display_if.reset = bsp_st7701s_reset;
    display_if.set_brightness = bsp_lcd_set_brightness;
    display_if.flush_dma = bsp_st7701s_flush_dma;

    *interface = &display_if;
    g_display_if = &display_if;
    g_display_info = &display_info;
    return true;
}
