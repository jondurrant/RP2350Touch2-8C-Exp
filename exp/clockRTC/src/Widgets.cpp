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
	initTile2();
	initTile3();

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
			lv_color_make(0x10, 0x40, 0xFF));

	uint hor = lv_disp_get_hor_res(NULL);
	uint ver = lv_disp_get_ver_res(NULL);
	pMeter = lv_meter_create(xTile1);
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
			6,
			lv_color_hex(0x404090),
			-35);

	pIndicHour = lv_meter_add_needle_line(
			pMeter,
			scale_min,
			8,
			lv_color_hex(0x404090),
			-100);

	pIndicSec = lv_meter_add_needle_line(
				pMeter,
				scale_min,
				1,
				lv_color_hex(0xF00000),
				-35);

	pDateLbl = lv_label_create(xTile1);
    lv_label_set_text(pDateLbl, "00-00-0000");
    lv_obj_set_width(pDateLbl, 300);
    lv_obj_set_style_text_align(pDateLbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pDateLbl, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_style(pDateLbl , &xLabelSt,  LV_PART_MAIN);

    /* Time on dial
	pTimeLbl = lv_label_create(xTile1);
    lv_label_set_text(pTimeLbl, "00:00:00");
    lv_obj_set_width(pTimeLbl, 300);
    lv_obj_set_style_text_align(pTimeLbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pTimeLbl, LV_ALIGN_CENTER, 0, -80);
    lv_obj_add_style(pTimeLbl , &xLabelSt,  LV_PART_MAIN);
    */

    pHumidLbl = lv_label_create(xTile1);
    lv_label_set_text(pHumidLbl, "0.0%");
    lv_obj_set_width(pHumidLbl, 300);
    lv_obj_set_style_text_align(pHumidLbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pHumidLbl, LV_ALIGN_CENTER, 0, 80);
    lv_obj_add_style(pHumidLbl , &xLabelSt,  LV_PART_MAIN);

    pTempLbl = lv_label_create(xTile1);
    lv_label_set_text(pTempLbl, "0.00C");
    lv_obj_set_width(pTempLbl, 300);
    lv_obj_set_style_text_align(pTempLbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pTempLbl, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_style(pTempLbl , &xLabelSt,  LV_PART_MAIN);


	pTimer = lv_timer_create(timerCB, 100,  this);

}


void Widgets::timerCB(lv_timer_t * timer){
	Widgets *self = (Widgets *)timer->user_data;
	self->timerHandler(timer);
}

void Widgets::timerHandler(lv_timer_t * timer){
	uint16_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t h;
	uint8_t m;
	uint8_t s;
	char buf[12];

	getDateTime( &year,  &mon,  &day,  &h,  &m,  &s);

	sprintf(buf, "%02d-%02d-%04d", day, mon, year);
	lv_label_set_text(pDateLbl, buf);

	if (pTimeLbl != NULL){
		sprintf(buf, "%02d:%02d:%02d", h, m, s);
		lv_label_set_text(pTimeLbl, buf);
	}

	if (h == 0){
		h = 12;
	}
	if (h > 12){
		h = h - 12;
	}
	showTime(h, m, s);

	if (pAHT != NULL){
		float t = pAHT->AHT10_readTemperature(true);
		if (t != AHT10_ERROR) {
			sprintf(buf, "%02.1fC", t);
			lv_label_set_text(pTempLbl, buf);
		}
		t = pAHT->AHT10_readHumidity(true);
		if (t != AHT10_ERROR) {
			sprintf(buf, "%.1f%%", t);
			lv_label_set_text(pHumidLbl, buf);
		}
	}
}

void Widgets::showTime(uint8_t h, uint8_t m, uint8_t s){
	if ((h >= 1) && (h <= 12)){
		if (m < 60){
			lv_meter_set_indicator_end_value(pMeter, pIndicMin, m);

			float hh = (float) h / 12.0 * 60 + (float) m / 60.0 * 5.0;
			if (hh > 60.0){
				hh = hh - 60.0;
			}

			lv_meter_set_indicator_end_value(pMeter, pIndicHour, (int)hh);

			if (s < 60){
				lv_meter_set_indicator_end_value(pMeter, pIndicSec, (int)s);
			}
		}
	}
}


void Widgets::initTile2() {

	struct tm now;
	xTile2 = lv_tileview_add_tile(xTV, 0, 1,  LV_DIR_ALL);

	uint hor = lv_disp_get_hor_res(NULL);
	uint ver = lv_disp_get_ver_res(NULL);

	pCalendar = lv_calendar_create(xTile2);
	lv_obj_set_size(pCalendar, hor-90, ver-90);
	lv_obj_align(pCalendar, LV_ALIGN_CENTER, 0, 27);
	lv_obj_add_event_cb(pCalendar, calendarCB, LV_EVENT_ALL,  this);

	bsp_pcf85063_get_time(&now);
	if ((now.tm_year+1900) < 2025){
		now.tm_year = 2025 -1900;
	}
	lv_calendar_set_today_date(pCalendar, now.tm_year+1900, now.tm_mon + 1, now.tm_mday);
	lv_calendar_set_showed_date(pCalendar, now.tm_year+1900, now.tm_mon + 1);

	//lv_calendar_header_dropdown_create(pCalendar);
	lv_calendar_header_arrow_create(pCalendar);

}

void Widgets::calendarCB(lv_event_t * e){
	Widgets *self = (Widgets *)lv_event_get_user_data(e);
	self->calendarHandler(e);
}


void Widgets::calendarHandler(lv_event_t * e){

	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_current_target(e);

	if(code == LV_EVENT_VALUE_CHANGED) {
		lv_calendar_date_t date;
		if(lv_calendar_get_pressed_date(obj, &date)) {
			setDateTime( date.year, date.month, date.day);
			lv_calendar_set_today_date(pCalendar, date.year, date.month, date.day);

			printf("Clicked date: %02d.%02d.%d\n", date.day, date.month, date.year);
		}
	}
}

void Widgets::initTile3() {

	xTile3 = lv_tileview_add_tile(xTV, 0, 2,  LV_DIR_ALL);

	lv_style_init(&xRollerSt);
	lv_style_set_text_font(&xRollerSt, &lv_font_montserrat_32);

	const char * optsH1 = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12";
	pRollerH1 = lv_roller_create(xTile3);
	lv_roller_set_options(pRollerH1, optsH1, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(pRollerH1, 2);
	lv_obj_set_width(pRollerH1, 100);
	lv_obj_add_style(pRollerH1, &xRollerSt, LV_PART_SELECTED);
	lv_obj_set_style_text_align(pRollerH1, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_align(pRollerH1, LV_ALIGN_LEFT_MID, 10, 0);

	const char * optsM1 = "0\n1\n2\n3\n4\n5";
	pRollerM1 = lv_roller_create(xTile3);
	lv_roller_set_options(pRollerM1, optsM1, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(pRollerM1, 2);
	lv_obj_set_width(pRollerM1, 100);
	lv_obj_add_style(pRollerM1, &xRollerSt, LV_PART_SELECTED);
	lv_obj_set_style_text_align(pRollerM1, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_align(pRollerM1, LV_ALIGN_LEFT_MID, 110, 0);

	const char * optsM2 = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
	pRollerM2 = lv_roller_create(xTile3);
	lv_roller_set_options(pRollerM2, optsM2, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(pRollerM2, 2);
	lv_obj_set_width(pRollerM2, 100);
	lv_obj_add_style(pRollerM2, &xRollerSt, LV_PART_SELECTED);
	lv_obj_set_style_text_align(pRollerM2, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_align(pRollerM2, LV_ALIGN_LEFT_MID, 210, 0);

	pCheckPM = lv_checkbox_create(xTile3);
    lv_checkbox_set_text(pCheckPM, "pm");
    lv_obj_align(pCheckPM, LV_ALIGN_LEFT_MID, 350, 0);
    lv_obj_add_style(pCheckPM , &xLabelSt,  LV_PART_MAIN);

    pTimeBtn  = lv_btn_create(xTile3);
    lv_obj_add_event_cb(pTimeBtn, setTimeCB, LV_EVENT_ALL, this);
    lv_obj_align(pTimeBtn, LV_ALIGN_CENTER, 0, 80);

    pSetTimeLbl = lv_label_create(pTimeBtn);
    lv_label_set_text(pSetTimeLbl, "Set Time");
    lv_obj_center(pSetTimeLbl);
    lv_obj_add_style(pSetTimeLbl , &xLabelSt,  LV_PART_MAIN);

}


void Widgets::setTimeCB(lv_event_t * e){
	Widgets *self = (Widgets *)lv_event_get_user_data(e);
	self-> setTimeHandler(e);
}

void Widgets::setTimeHandler(lv_event_t * e){
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED) {
		uint8_t hour = lv_roller_get_selected(pRollerH1) + 1;
		uint8_t min = lv_roller_get_selected(pRollerM1) * 10 + lv_roller_get_selected(pRollerM2);
		if (lv_obj_get_state(pCheckPM) & LV_STATE_CHECKED){
			hour = hour + 12;
		}

		printf("Set time to %02d:%02d\n", hour, min);
		setDateTime( 0xffff,  0xff,  0xff,  hour,  min, 0 );
	}
}


void Widgets::getDateTime(uint16_t *year, uint8_t *mon, uint8_t *day, uint8_t *h, uint8_t *m, uint8_t *s){
	struct tm now;

	now.tm_mon = -1;
	while (now.tm_mon < 0){
		bsp_pcf85063_get_time(&now);
		sleep_ms(1);
	}
	*year = now.tm_year + 1900;
	*mon = now.tm_mon + 1;
	*day = now.tm_mday;
	if (*year < 2025){
		*year = 2025;
	}
	if  ((*mon < 1) || (*mon > 12) ){
		*mon = 1;
	}
	if  ((*day < 1) || (*day > 31) ){
		*day = 1;
	}

	*h = now.tm_hour;
	*m = now.tm_min;
	*s = now.tm_sec;
	if ((*h < 1) || (*h > 23)){
		*h = 12;
	}
	if ((*m > 60)){
		*m = 0;
	}
	if ((*s > 60)){
		*s = 0;
	}
}


void Widgets::setDateTime(uint16_t year, uint8_t mon, uint8_t day, uint8_t h, uint8_t m, uint8_t s){
	struct tm now;

	printf("setDateTime(%04u,%02u,%02u,%02u,%02u,%02u)\n",
			year,
			mon,
			day,
			h,
			m,
			s);

	//Get current values
	now.tm_mon = -1;
	while (now.tm_mon < 0){
		bsp_pcf85063_get_time(&now);
		sleep_ms(1);
	}
	printf("TM %02d-%02d-%04d %02d:%02d:%02d\n",
			now.tm_mday, now.tm_mon, now.tm_year,
			now.tm_hour, now.tm_min , now.tm_sec);

	//Copy day date in if not set to ignore
	if (year != 0xFFFF){
		now.tm_year = year - 1900;
	}
	if (mon != 0xFF){
		now.tm_mon = mon - 1;
	}
	if (day != 0xFF){
		now.tm_mday = day;
	}

	//Validate date
	if ((now.tm_mon > 11) || (now.tm_mon < 0) ){
		now.tm_mon = 0;
	}
	if ((now.tm_mday < 1) || (now.tm_mday > 31)){
		now.tm_mday = 1;
	}

	//Copy in time
	printf("Time was %02u:%02u:%02u\n", now.tm_hour, now.tm_min , now.tm_sec);
	if (h != 0xFF){
		now.tm_hour = h;
	}
	if (m != 0xFF){
		now.tm_min = m;
	}
	if (s != 0xFF){
		now.tm_sec = s;
	}
	printf("Time now %02u:%02u:%02u\n", now.tm_hour, now.tm_min , now.tm_sec);

	//Validate time
	if (now.tm_hour > 23){
		now.tm_hour = 0;
	}
	if (now.tm_min >=60){
		now.tm_min = 0;
	}
	if (now.tm_sec >=60){
		now.tm_sec = 0;
	}

	printf("Setting Date Time %02d-%02d-%04d %02d:%02d:%02d\n",
			now.tm_mday,
			now.tm_mon,
			now.tm_year,
			now.tm_hour,
			now.tm_min,
			now.tm_sec);

	bsp_pcf85063_set_time(&now);
}


void Widgets::setAHT10(LIB_AHTXX* p){
	pAHT = p;
}


