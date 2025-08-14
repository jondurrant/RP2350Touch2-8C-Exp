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


private:

	static void timerCB(lv_timer_t * timer);
	void timerHandler(lv_timer_t * timer);

	void initTile1();

	lv_obj_t *xTV;
	lv_obj_t *xTile1;
	lv_style_t xStyleTile;
	lv_style_t xLabelSt;

	lv_obj_t * pDrJonLbl;

	lv_obj_t * pArcs[NUM_ARCS];

	lv_timer_t * pTimer;


	uint16_t xArcsStart[NUM_ARCS] = {0, 45, 90, 135, 180, 225};


};

#endif /* EXP_LVGLDASHBOARD_SRC_WIDGETS_H_ */
