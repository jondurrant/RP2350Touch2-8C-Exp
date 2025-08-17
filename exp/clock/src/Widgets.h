/*
 * Widgets.h
 *
 *  Created on: 4 Aug 2025
 *      Author: jondurrant
 */

#ifndef EXP_LVGLDASHBOARD_SRC_WIDGETS_H_
#define EXP_LVGLDASHBOARD_SRC_WIDGETS_H_

#include "lvgl.h"
#include "src/core/lv_obj.h"
#include "src/misc/lv_area.h"
#include "pico/stdlib.h"


#define NUM_ARCS 6


class Widgets {
public:
	Widgets();
	virtual ~Widgets();

	void init();

	void setTime(uint8_t h, uint8_t m);


private:

	void initTile1();

	static void timerCB(lv_timer_t * timer);
	void timerHandler(lv_timer_t * timer);


	lv_obj_t *xTV;
	lv_obj_t *xTile1;
	lv_style_t xStyleTile;
	lv_style_t xLabelSt;

	lv_obj_t * pMeter;


	lv_meter_indicator_t * pIndicMin;
	lv_meter_indicator_t * pIndicHour;

	lv_timer_t * pTimer;

	uint8_t xHour = 1;
	uint8_t xMin = 0;

};

#endif /* EXP_LVGLDASHBOARD_SRC_WIDGETS_H_ */
