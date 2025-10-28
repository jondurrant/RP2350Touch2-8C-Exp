/*
 * GuiClock.h
 *
 *  Created on: 6 Oct 2025
 *      Author: jondurrant
 */

#ifndef FIRMWARE_RP2350TOUCH2_8C_SRC_GUICLOCK_H_
#define FIRMWARE_RP2350TOUCH2_8C_SRC_GUICLOCK_H_

#include "lvgl.h"
#include "src/core/lv_obj.h"
#include "src/misc/lv_area.h"
#include "pico/stdlib.h"
#include <cstdio>

#define SEC_HAND 			0x0000FF
#define MIN_HAND 			0x0000AF
#define HOUR_HAND 		0x0000AF


class GuiClock {
public:
	GuiClock();
	virtual ~GuiClock();

	void init(lv_obj_t *parent);

	void showTime(uint8_t h, uint8_t m);
private:
	lv_obj_t * pMeter;
	lv_meter_indicator_t * pIndicMin;
	lv_meter_indicator_t * pIndicHour;

	lv_timer_t * pTimer;



};

#endif /* FIRMWARE_RP2350TOUCH2_8C_SRC_GUICLOCK_H_ */
