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


	pDrJonLbl = lv_label_create(xTile1);
	lv_label_set_long_mode(pDrJonLbl, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
	lv_label_set_text(pDrJonLbl, "DrJonEA");
	lv_obj_set_width(pDrJonLbl, 150);  /*Set smaller width to make the lines wrap*/
	lv_obj_set_style_text_align(pDrJonLbl, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(pDrJonLbl, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(pDrJonLbl , &xLabelSt,  LV_PART_MAIN);

	uint hor = lv_disp_get_hor_res(NULL);
	uint ver = lv_disp_get_ver_res(NULL);

	for (int i=0; i < NUM_ARCS; i++){
		pArcs[i] = lv_arc_create(xTile1);
		lv_obj_set_size(pArcs[i], hor - (45 * i), ver - (45 * i));
		lv_arc_set_rotation(pArcs[i], 270);
		lv_arc_set_bg_angles(pArcs[i], 0, 0);
		lv_obj_remove_style(pArcs[i], NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
		lv_obj_clear_flag( pArcs[i],  LV_OBJ_FLAG_CLICKABLE);
		lv_obj_set_style_arc_width(pArcs[i], 25, LV_PART_INDICATOR);
		lv_obj_center(pArcs[i]);

		lv_color_t c ;
		switch(i){
		case 0:
			c =  lv_color_make(0xE4, 0x03, 0x03);
			break;
		case 1:
			c =  lv_color_make(0xFF, 0x8C, 0);
			break;
		case 2:
			c =  lv_color_make(0xFF, 0xED, 0);
			break;
		case 3:
			c =  lv_color_make(0x00, 0x80, 0x26);
			break;
		case 4:
			c =  lv_color_make(0x0, 0x4C, 0xFF);
			break;
		case 5:
			c =  lv_color_make(0x73, 0x29, 0x82);
			break;
		default:
			c =  lv_color_make(0, 0, 0);
			break;
		}

		lv_obj_set_style_arc_color(
				pArcs[i],
				c,
				LV_PART_INDICATOR);
	}

	pTimer = lv_timer_create(timerCB, 1,  this);
}



void Widgets::timerCB(lv_timer_t * timer){
	Widgets *self = (Widgets *)timer->user_data;
	self->timerHandler(timer);
}

void Widgets::timerHandler(lv_timer_t * timer){
	for (int i=0; i < NUM_ARCS; i++){
		uint16_t r = xArcsStart[i];
		r = (r +  NUM_ARCS - i + 2) % 360;
		xArcsStart[i] = r;
		if ( (i % 2) == 0){
			lv_arc_set_rotation(pArcs[i], r);
		} else {
			lv_arc_set_rotation(pArcs[i], 360 - r);
		}
	}
}

