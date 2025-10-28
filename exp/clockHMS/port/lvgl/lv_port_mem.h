/*
 * lv_port_mem.h
 *
 *  Created on: 9 Oct 2025
 *      Author: jondurrant
 */

#ifndef FIRMWARE_RP2350TOUCH2_8C_PORT_LVGL_LV_PORT_MEM_H_
#define FIRMWARE_RP2350TOUCH2_8C_PORT_LVGL_LV_PORT_MEM_H_
#include "pico/stdlib.h"

void *port_malloc( size_t size );

void *port_realloc( void *ptr, size_t new_size );

void port_free( void *ptr );






#endif /* FIRMWARE_RP2350TOUCH2_8C_PORT_LVGL_LV_PORT_MEM_H_ */
