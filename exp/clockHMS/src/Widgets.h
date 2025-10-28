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
#include "GuiClock.h"


#define NUM_ARCS 6


class Widgets {
public:
	Widgets();
	virtual ~Widgets();

	void init();

private:
	static void timerCB(lv_timer_t * timer);
	void timerHandler(lv_timer_t * timer);


	lv_obj_t *pTV;

	lv_timer_t * pTimer;

	GuiClock clock;

	uint xSecs = 0;

};

#endif /* EXP_LVGLDASHBOARD_SRC_WIDGETS_H_ */
