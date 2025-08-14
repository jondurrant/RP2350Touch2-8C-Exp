#include "bsp_buzzer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define PWM_FREQ 1000
#define PWM_WRAP 1000

static uint slice_num;
static uint pwm_channel;

void bsp_buzzer_init(void)
{
    float sys_clk = clock_get_hz(clk_sys);
    gpio_set_function(BSP_BUZZER_PIN, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    slice_num = pwm_gpio_to_slice_num(BSP_BUZZER_PIN);

    pwm_channel = pwm_gpio_to_channel(BSP_BUZZER_PIN);

    pwm_set_clkdiv(slice_num, sys_clk / (PWM_FREQ * PWM_WRAP));

    pwm_set_wrap(slice_num, PWM_WRAP);

    pwm_set_chan_level(slice_num, pwm_channel, 0);

    pwm_set_enabled(slice_num, true);
}

void bsp_buzzer_enable(bool enable)
{
    if (enable)
        pwm_set_chan_level(slice_num, pwm_channel, PWM_WRAP / 2);
    else
        pwm_set_chan_level(slice_num, pwm_channel, 0);
}