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
	pTV = lv_tileview_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(pTV,  LV_SCROLLBAR_MODE_OFF);

	clock.init(pTV);

	pTimer = lv_timer_create(Widgets::timerCB, 10,  this);
}

void Widgets::timerCB(lv_timer_t * timer){
	Widgets *self = (Widgets *)timer->user_data;
	self->timerHandler(timer);
}
void Widgets::timerHandler(lv_timer_t * timer){
	uint s = xSecs % 60;
	uint m =(xSecs /  60) % 60;
	uint h =(xSecs /  60 / 60) + 1;

	clock.showTime(h, m, s);
	xSecs++;
	if (xSecs > (60* 60 * 12)){
		xSecs = 0;
	}
}







