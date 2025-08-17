/*
 * Widgets.cpp
 *
 *  Created on: 4 Aug 2025
 *      Author: jondurrant
 */

#include "Widgets.h"
#include <cstdio>



Widgets::Widgets() {

}

Widgets::~Widgets() {
	// TODO Auto-generated destructor stub
}



void Widgets::init() {

	 /*Create tileview*/
	xTV = lv_tileview_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(xTV,  LV_SCROLLBAR_MODE_OFF);

	initTile1();

}



void Widgets::initTile1() {

	/*Tile1: */
	xTile1 = lv_tileview_add_tile(xTV, 0, 0,  LV_DIR_ALL);


	lv_style_init(&xStyleTile);
	lv_style_set_bg_color(&xStyleTile, lv_color_hex(0x000000));
	lv_style_set_bg_opa(&xStyleTile, LV_OPA_COVER);
	//lv_obj_add_style(xTile1, &xStyleTile, 0);

	lv_style_init(&xLabelSt);
	lv_style_set_text_font(&xLabelSt, &lv_font_montserrat_32);
	lv_style_set_text_color(
			&xLabelSt,
			lv_color_make(0, 0x40, 0x40));

	uint hor = lv_disp_get_hor_res(NULL);
	uint ver = lv_disp_get_ver_res(NULL);
	pMeter = lv_meter_create(lv_scr_act());
	lv_obj_set_size(pMeter, hor, ver);
	lv_obj_center(pMeter);

	/*Create a scale for the minutes*/
	/*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
	lv_meter_scale_t * scale_min = lv_meter_add_scale(pMeter);
	lv_meter_set_scale_ticks(pMeter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
	lv_meter_set_scale_range(pMeter, scale_min, 0, 60, 360, 270);

	/*Create another scale for the hours. It's only visual and contains only major ticks*/
	lv_meter_scale_t * scale_hour = lv_meter_add_scale(pMeter);
	lv_meter_set_scale_ticks(pMeter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));               /*12 ticks*/
	lv_meter_set_scale_major_ticks(pMeter, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
	lv_meter_set_scale_range(pMeter, scale_hour, 1, 12, 330, 300);       /*[1..12] values in an almost full circle*/

	pIndicMin = lv_meter_add_needle_line(
			pMeter,
			scale_min,
			2,
			lv_palette_main(LV_PALETTE_GREY),
			-35);

	pIndicHour = lv_meter_add_needle_line(
			pMeter,
			scale_min,
			4,
			lv_color_hex(0x000000),
			-100);


	pTimer = lv_timer_create(timerCB, 100,  this);

}


void Widgets::timerCB(lv_timer_t * timer){
	Widgets *self = (Widgets *)timer->user_data;
	self->timerHandler(timer);
}

void Widgets::timerHandler(lv_timer_t * timer){
	xMin ++;
	if (xMin >= 60){
		xMin = 0;
		xHour ++;
		if (xHour > 12){
			xHour = 1;
		}
	}

	setTime(xHour, xMin);
}

void Widgets::setTime(uint8_t h, uint8_t m){
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


