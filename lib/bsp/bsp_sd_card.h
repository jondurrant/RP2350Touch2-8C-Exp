#ifndef __BSP_SD_CARD_H__
#define __BSP_SD_CARD_H__

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool bsp_sd_card_init(void);
void bsp_sd_card_deinit(void);
void bsp_sd_card_test(void);
uint32_t bsp_sd_card_get_size(void);
#ifdef __cplusplus
}
#endif

#endif