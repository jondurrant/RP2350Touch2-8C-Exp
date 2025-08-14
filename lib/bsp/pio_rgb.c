#include "pio_rgb.h"
#include "pio_rgb.pio.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"

#define RGB_SYNC_PIO pio1
#define RGB_COLOR_DATA_PIO pio2
#define RGB_PIO_BASE_PIN 16

static uint vsync_sm;
static uint hsync_sm;

static uint rgb_de_sm;
static uint rgb_sm;

static int rgb_dma_chan;

static pio_rgb_info_t *g_pio_rgb_info;
static uint16_t *buffer;

uint16_t test_count = 0;
void __no_inline_not_in_flash_func(dma_complete_handler)(void)
{

    test_count = (test_count + 1) % g_pio_rgb_info->transfer_index_max;
    // 双缓存模式 double buffer mode
    if (g_pio_rgb_info->mode.double_buffer)
    {
        if (g_pio_rgb_info->mode.enabled_transfer)
        {
            // 更新 transfer_index
            g_pio_rgb_info->transfer_index = (g_pio_rgb_info->transfer_index + 1) % g_pio_rgb_info->transfer_index_max;

            // 获取当前帧缓冲区指针
            uint16_t *transfer_buffer_p = &g_pio_rgb_info->_framebuffer[g_pio_rgb_info->transfer_index * g_pio_rgb_info->transfer_size];

            if (g_pio_rgb_info->mode.enabled_psram) // 使用psram
            {
                // 根据奇偶选择缓冲区
                uint16_t *dma_buffer = (g_pio_rgb_info->transfer_index % 2)
                                           ? g_pio_rgb_info->transfer_buffer1
                                           : g_pio_rgb_info->transfer_buffer2;

                uint16_t *cp_buffer = (g_pio_rgb_info->transfer_index % 2)
                                          ? g_pio_rgb_info->transfer_buffer2
                                          : g_pio_rgb_info->transfer_buffer1;
                // 设置 DMA 读取地址
                dma_channel_set_read_addr(rgb_dma_chan, dma_buffer, true);

                // 将数据拷贝到 transfer_buffer
                // memcmp(cp_buffer, transfer_buffer_p, g_pio_rgb_info->transfer_size * sizeof(uint16_t));
                for (size_t i = 0; i < g_pio_rgb_info->transfer_size; i++)
                {
                    cp_buffer[i] = transfer_buffer_p[i];
                }
            }
            else // 使用sram不使用psram
            {
                dma_channel_set_read_addr(rgb_dma_chan, transfer_buffer_p, true);
            }
            // 刷新完成
            if (g_pio_rgb_info->change_framebuffer_flag && (g_pio_rgb_info->transfer_index == g_pio_rgb_info->transfer_index_max - 1))
            {
                g_pio_rgb_info->change_framebuffer_flag = false;
                // 切换缓冲区
                g_pio_rgb_info->_framebuffer = (g_pio_rgb_info->_framebuffer == g_pio_rgb_info->framebuffer1)
                                                   ? g_pio_rgb_info->framebuffer2
                                                   : g_pio_rgb_info->framebuffer1;

                if (g_pio_rgb_info->dma_flush_done_cb)
                {
                    g_pio_rgb_info->dma_flush_done_cb();
                }
            }
        }
        else
        {
            dma_channel_set_read_addr(rgb_dma_chan, g_pio_rgb_info->_framebuffer, true);
            if (g_pio_rgb_info->change_framebuffer_flag)
            {
                g_pio_rgb_info->change_framebuffer_flag = false;
                if (g_pio_rgb_info->dma_flush_done_cb)
                {
                    g_pio_rgb_info->dma_flush_done_cb();
                }
            }
        }
    }
    else // 单缓存模式，enabled_transfer
    {
        if (g_pio_rgb_info->mode.enabled_transfer)
        {

            if (g_pio_rgb_info->mode.enabled_psram) // 使用psram
            {
                // 更新 transfer_index
                g_pio_rgb_info->transfer_index = (g_pio_rgb_info->transfer_index + 1) % g_pio_rgb_info->transfer_index_max;
                // 获取当前帧缓冲区指针
                uint16_t *transfer_buffer_p = &g_pio_rgb_info->_framebuffer[g_pio_rgb_info->transfer_index * g_pio_rgb_info->transfer_size];
                // 根据奇偶选择缓冲区
                uint16_t *dma_buffer = (g_pio_rgb_info->transfer_index % 2)
                                           ? g_pio_rgb_info->transfer_buffer1
                                           : g_pio_rgb_info->transfer_buffer2;

                uint16_t *cp_buffer = (g_pio_rgb_info->transfer_index % 2)
                                          ? g_pio_rgb_info->transfer_buffer2
                                          : g_pio_rgb_info->transfer_buffer1;
                // 设置 DMA 读取地址
                dma_channel_set_read_addr(rgb_dma_chan, dma_buffer, true);

                for (size_t i = 0; i < g_pio_rgb_info->transfer_size; i++)
                {
                    cp_buffer[i] = transfer_buffer_p[i];
                }
            }
            else
            {
                uint16_t *transfer_buffer_p = &g_pio_rgb_info->_framebuffer[g_pio_rgb_info->transfer_index * g_pio_rgb_info->transfer_size];
                g_pio_rgb_info->transfer_index = (g_pio_rgb_info->transfer_index + 1) % g_pio_rgb_info->transfer_index_max;
                // 设置 DMA 读取地址
                dma_channel_set_read_addr(rgb_dma_chan, transfer_buffer_p, true);
            }
            // 刷新完成
            if ((g_pio_rgb_info->transfer_index == g_pio_rgb_info->transfer_index_max - 1) && g_pio_rgb_info->dma_flush_done_cb)
            {
                g_pio_rgb_info->dma_flush_done_cb();
            }
        }
        else
        {
            dma_channel_set_read_addr(rgb_dma_chan, g_pio_rgb_info->_framebuffer, true);
            if (g_pio_rgb_info->dma_flush_done_cb)
            {
                g_pio_rgb_info->dma_flush_done_cb();
            }
        }
    }
}

/**
 * @brief 切换帧缓冲区
 */
void pio_rgb_change_framebuffer(void)
{
    g_pio_rgb_info->change_framebuffer_flag = true;
}

/**
 * @brief 获取空闲的帧缓冲区
 */

uint16_t *pio_rgb_get_free_framebuffer(void)
{
    if (g_pio_rgb_info->mode.double_buffer)
    {
        return (g_pio_rgb_info->_framebuffer == g_pio_rgb_info->framebuffer1) ? g_pio_rgb_info->framebuffer2 : g_pio_rgb_info->framebuffer1;
    }
    return g_pio_rgb_info->_framebuffer;
}

/**
 * @brief 更新帧缓冲区的内容
 */
void pio_rgb_update_framebuffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *color_p)
{
    size_t color_width = (x2 - x1 + 1);
    size_t color_height = (y2 - y1 + 1);
    for (size_t i = 0; i < color_height; i++)
    {
        // uint16_t index =  (i + y1) / (g_pio_rgb_info->transfer_size / g_pio_rgb_info->width);
        // uint16_t index_next = (index + 1) % g_pio_rgb_info->transfer_index_max;
        // do
        // {
        //     if (g_pio_rgb_info->transfer_index != index_next && g_pio_rgb_info->transfer_index != index)
        //     {
        //         break;
        //     }
        //     sleep_us(1);
        // } while (1);
        
        for (size_t j = 0; j < color_width; j++)
        {
            g_pio_rgb_info->_framebuffer[(i + y1) * g_pio_rgb_info->width + (j + x1)] = color_p[i * color_width + j];
        }
    }
}

static inline void hsync_program_init(PIO pio, uint sm, uint offset, uint pin, float div)
{
    // creates state machine configuration object c, sets
    // to default configurations. I believe this function is auto-generated
    // and gets a name of <program name>_program_get_default_config
    pio_sm_config c = hsync_program_get_default_config(offset);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // Map the state machine's SET pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_sideset_pins(&c, pin);

    // Set clock division (div by 5 for 25 MHz state machine)
    sm_config_set_clkdiv(&c, div);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin + 1);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 2, true);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Set the state machine running (commented out so can be synchronized w/ vsync)
    // pio_sm_set_enabled(pio, sm, true);
}

static inline void vsync_program_init(PIO pio, uint sm, uint offset, uint pin, float div)
{

    // creates state machine configuration object c, sets
    // to default configurations. I believe this function is auto-generated
    // and gets a name of <program name>_program_get_default_config
    pio_sm_config c = vsync_program_get_default_config(offset);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // Map the state machine's SET pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_sideset_pins(&c, pin);

    // Set clock division (div by 5 for 25 MHz state machine)
    sm_config_set_clkdiv(&c, div);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Set the state machine running (commented out so can be synchronized with hsync)
    // pio_sm_set_enabled(pio, sm, true);
}

static inline void rgb_de_program_init(PIO pio, uint sm, uint offset, uint pin, float div)
{

    // creates state machine configuration object c, sets
    // to default configurations. I believe this function is auto-generated
    // and gets a name of <program name>_program_get_default_config
    pio_sm_config c = rgb_de_program_get_default_config(offset);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // Map the state machine's SET pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_sideset_pins(&c, pin);

    // Set clock division (div by 5 for 25 MHz state machine)
    sm_config_set_clkdiv(&c, div);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Set the state machine running (commented out so can be synchronized with hsync)
    // pio_sm_set_enabled(pio, sm, true);
}

static inline void rgb_program_init(PIO pio, uint sm, uint offset, uint pin, float div)
{

    // creates state machine configuration object c, sets
    // to default configurations. I believe this function is auto-generated
    // and gets a name of <program name>_program_get_default_config
    pio_sm_config c = rgb_program_get_default_config(offset);
    // sm_config_set_out_shift(&c, true, false, 32);
    // Map the state machine's SET and OUT pin group to 16 pins, the `pin`
    // parameter to this function is the lowest one. These groups overlap.
    // sm_config_set_set_pins(&c, pin, 16);
    // 设置out的引脚
    sm_config_set_out_pins(&c, pin, 16);
    // // 设置sideset的引脚
    // sm_config_set_sideset_pins(&c, pin + 16);

    // Set clock division (Commented out, this one runs at full speed)
    // 设置分频系数
    sm_config_set_clkdiv(&c, div);

    // Set this pin's GPIO function (connect PIO to the pad)
    // 设置引脚为pio引脚
    for (int i = 0; i < 16; i++)
    {
        pio_gpio_init(pio, pin + i);
        gpio_pull_up(pin + i);
    }

    // Set the pin direction to output at the PIO (4 pins)
    // 设置输出的引脚
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 16, true);

    // Load our configuration, and jump to the start of the program
    // 初始化状态机
    pio_sm_init(pio, sm, offset, &c);

    // Set the state machine running (commented out, I'll start this in the C)
    // pio_sm_set_enabled(pio, sm, true);
}

void pio_rgb_dma_init(pio_rgb_info_t *info)
{
    dma_channel_config c0 = dma_channel_get_default_config(rgb_dma_chan);
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_16); // 16-bit transfers
    channel_config_set_read_increment(&c0, true);
    channel_config_set_write_increment(&c0, false);
    channel_config_set_dreq(&c0, pio_get_dreq(RGB_COLOR_DATA_PIO, rgb_sm, true)); // set for pio2 sm 0
    // channel_config_set_chain_to(&c0, rgb_chan_1);

    uint transfer_count = (info->mode.enabled_transfer) ? info->transfer_size : (info->width * info->height);

    dma_channel_configure(
        rgb_dma_chan,
        &c0,
        &RGB_COLOR_DATA_PIO->txf[rgb_sm], // RGB PIO2 TX FIFO
        NULL,                             // frame buffer
        transfer_count,                   // size of frame buffer
        false);
    bsp_dma_channel_irq_add(1, rgb_dma_chan, dma_complete_handler);
}

void pio_rgb_init(pio_rgb_info_t *info, pio_rgb_pin_t *pin)
{
    static pio_rgb_info_t pio_rgb_info;
    memcpy(&pio_rgb_info, info, sizeof(pio_rgb_info_t));
    g_pio_rgb_info = &pio_rgb_info;
    g_pio_rgb_info->change_framebuffer_flag = false;
    g_pio_rgb_info->_framebuffer = g_pio_rgb_info->framebuffer1;
    g_pio_rgb_info->transfer_index = 0;
    g_pio_rgb_info->transfer_index_max = g_pio_rgb_info->width * g_pio_rgb_info->height / g_pio_rgb_info->transfer_size;
    // printf("transfer_index_max:%d\n", g_pio_rgb_info->transfer_index_max);
    // printf("pio_rgb_init framebuffer1:0x%x\r\n", g_pio_rgb_info->framebuffer1);
    // printf("pio_rgb_init framebuffer2:0x%x\r\n", g_pio_rgb_info->framebuffer2);
    // printf("pio_rgb_init _framebuffer:0x%x\r\n", g_pio_rgb_info->_framebuffer);

    // 获取系统时钟
    float sys_clk = clock_get_hz(clk_sys);
    float pio_freq = sys_clk / ((float)(info->pclk_freq * 2));

    pio_set_gpio_base(RGB_SYNC_PIO, RGB_PIO_BASE_PIN);
    pio_set_gpio_base(RGB_COLOR_DATA_PIO, RGB_PIO_BASE_PIN);
    hsync_sm = pio_claim_unused_sm(RGB_SYNC_PIO, true);
    vsync_sm = pio_claim_unused_sm(RGB_SYNC_PIO, true);
    rgb_de_sm = pio_claim_unused_sm(RGB_COLOR_DATA_PIO, true);
    rgb_sm = pio_claim_unused_sm(RGB_COLOR_DATA_PIO, true);

    // 添加pio 程序
    uint hsync_offset = pio_add_program(RGB_SYNC_PIO, &hsync_program);
    uint vsync_offset = pio_add_program(RGB_SYNC_PIO, &vsync_program);
    uint rgb_de_offset = pio_add_program(RGB_COLOR_DATA_PIO, &rgb_de_program);
    uint rgb_offset = pio_add_program(RGB_COLOR_DATA_PIO, &rgb_program);

    // 初始化pio 程序
    hsync_program_init(RGB_SYNC_PIO, hsync_sm, hsync_offset, pin->hsync_pin, pio_freq);
    vsync_program_init(RGB_SYNC_PIO, vsync_sm, vsync_offset, pin->vsync_pin, 1.0f);
    rgb_de_program_init(RGB_COLOR_DATA_PIO, rgb_de_sm, rgb_de_offset, pin->de_pin, 1.0f);
    rgb_program_init(RGB_COLOR_DATA_PIO, rgb_sm, rgb_offset, pin->data0_pin, 1.0f);

    pio_rgb_dma_init(info);
    pio_sm_put_blocking(RGB_SYNC_PIO, hsync_sm, info->width - 1);

    pio_sm_put_blocking(RGB_SYNC_PIO, vsync_sm, info->height - 1);
    
    pio_sm_put_blocking(RGB_COLOR_DATA_PIO, rgb_de_sm, info->height - 1);
    pio_sm_put_blocking(RGB_COLOR_DATA_PIO, rgb_sm, info->width - 1);

    pio_enable_sm_mask_in_sync(RGB_COLOR_DATA_PIO, ((1u << rgb_de_sm) | (1u << rgb_sm)));
    pio_enable_sm_mask_in_sync(RGB_SYNC_PIO, ((1u << hsync_sm) | (1u << vsync_sm)));

    dma_complete_handler();
}
