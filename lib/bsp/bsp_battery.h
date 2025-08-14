#ifndef __BSP_BATTERY_H__
#define __BSP_BATTERY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#define BSP_BAT_ADC_PIN     40

void bsp_battery_init(void);
void bsp_battery_read(float *voltage, uint16_t *adc_raw);

#endif // __BSP_BATTERY_H__

