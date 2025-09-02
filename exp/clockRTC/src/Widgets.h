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

#include "ahtxx/ahtxx.hpp"


extern "C"{
#include "bsp_pcf85063.h"
}


#define NUM_ARCS 6


class Widgets {
public:
	Widgets();
	virtual ~Widgets();

	void init();

	void showTime(uint8_t h, uint8_t m, uint8_t s);


	void getDateTime(uint16_t *year, uint8_t *mon, uint8_t *day, uint8_t *h, uint8_t *m, uint8_t *s);
	void setDateTime(uint16_t year=0xffff, uint8_t mon=0xff, uint8_t day=0xff, uint8_t h=0xff, uint8_t m=0xff, uint8_t s=0xff);

	void setAHT10(LIB_AHTXX *aht10);

private:

	void initTile1();
	void initTile2();
	void initTile3();

	static void timerCB(lv_timer_t * timer);
	void timerHandler(lv_timer_t * timer);

	static void calendarCB(lv_event_t * e);
	void calendarHandler(lv_event_t * e);

	static void setTimeCB(lv_event_t * e);
	void setTimeHandler(lv_event_t * e);

	lv_obj_t *xTV;
	lv_obj_t *xTile1;
	lv_obj_t *xTile2;
	lv_obj_t *xTile3;
	lv_style_t xStyleTile;
	lv_style_t xLabelSt;
	lv_style_t xRollerSt;

	lv_obj_t * pMeter;

	lv_meter_indicator_t * pIndicMin;
	lv_meter_indicator_t * pIndicHour;
	lv_meter_indicator_t * pIndicSec;

	lv_obj_t * pDateLbl;
	lv_obj_t * pTimeLbl = NULL;
	lv_obj_t * pTempLbl;
	lv_obj_t * pHumidLbl;

	lv_timer_t * pTimer;

	lv_obj_t * pCalendar;

	uint8_t xHour = 1;
	uint8_t xMin = 0;

	lv_obj_t * pRollerH1;
	lv_obj_t * pRollerM1;
	lv_obj_t * pRollerM2;
	lv_obj_t * pCheckPM;
	lv_obj_t * pTimeBtn;
	lv_obj_t *pSetTimeLbl;

	LIB_AHTXX *pAHT = NULL;

};

#endif /* EXP_LVGLDASHBOARD_SRC_WIDGETS_H_ */
