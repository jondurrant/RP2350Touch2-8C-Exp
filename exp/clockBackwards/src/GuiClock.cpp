/*
 * GuiClock.cpp
 *
 *  Created on: 6 Oct 2025
 *      Author: jondurrant
 */

#include "GuiClock.h"


GuiClock::GuiClock() {
	// TODO Auto-generated constructor stub

}

GuiClock::~GuiClock() {
	// TODO Auto-generated destructor stub
}

void GuiClock::init(lv_obj_t *parent){
	uint hor = lv_disp_get_hor_res(NULL);
	uint ver = lv_disp_get_ver_res(NULL);
	pMeter = lv_meter_create(parent);
	lv_obj_set_size(pMeter, hor, ver);
	lv_obj_center(pMeter);

	/*Create a scale for the minutes*/
	/*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
	lv_meter_scale_t * scale_min = lv_meter_add_scale(pMeter);
	lv_meter_set_scale_ticks(pMeter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
	lv_meter_set_scale_range(pMeter, scale_min, 60, 0, 360, 270);

	/*Create another scale for the hours. It's only visual and contains only major ticks*/
	lv_meter_scale_t * scale_hour = lv_meter_add_scale(pMeter);
	lv_meter_set_scale_ticks(pMeter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));               /*12 ticks*/
	lv_meter_set_scale_major_ticks(pMeter, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
	lv_meter_set_scale_range(pMeter, scale_hour, 12, 1, 330, 300);       /*[1..12] values in an almost full circle*/

	pIndicMin = lv_meter_add_needle_line(
			pMeter,
			scale_min,
			6,
			lv_color_hex(MIN_HAND),
			-40);

	pIndicHour = lv_meter_add_needle_line(
			pMeter,
			scale_min,
			12,
			lv_color_hex(HOUR_HAND),
			-80);
}



void GuiClock::showTime(uint8_t h, uint8_t m){
	if ((h >= 1) && (h <= 12)){
		if (m < 60){
			lv_meter_set_indicator_end_value(pMeter, pIndicMin, m);

			float hh = (float) h / 12.0 * 60 + (float) m / 60.0 * 5.0;
			if (hh > 60.0){
				hh = hh - 60.0;
			}

			lv_meter_set_indicator_end_value(pMeter, pIndicHour, (int)hh);

		}
	}
}






