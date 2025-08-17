#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"



#include "hardware/clocks.h"
extern "C"{
#include "bsp_i2c.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "bsp_qmi8658.h"
#include "bsp_pcf85063.h"
#include "bsp_battery.h"
#include "bsp_buzzer.h"
}

#include "Widgets.h"


#define LVGL_TICK_PERIOD_MS 10
void set_cpu_clock(uint32_t freq_Mhz){
    set_sys_clock_khz(freq_Mhz * 1000, true);
    clock_configure(
        clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
        freq_Mhz * 1000 * 1000,
        freq_Mhz * 1000 * 1000);
}

bool repeating_lvgl_timer_cb(struct repeating_timer *t){
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
    return true;
}

int main()
{


    struct repeating_timer lvgl_timer;
	set_cpu_clock(240);
    stdio_init_all();
    sleep_ms(2000);

	bsp_i2c_init();
	//bsp_battery_init();
	//bsp_qmi8658_init();
	//bsp_pcf85063_init();
	//bsp_buzzer_init();

	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	add_repeating_timer_ms(LVGL_TICK_PERIOD_MS, repeating_lvgl_timer_cb, NULL, &lvgl_timer);

    Widgets widgets;

    printf("Pre Widgets\n");
    widgets.init();
    for(;;){
        lv_timer_handler();
        //printf("Loop\n");
        //sleep_ms(1);
    }
}
