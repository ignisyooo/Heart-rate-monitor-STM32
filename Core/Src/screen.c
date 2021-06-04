/*
 * screen.c
 *
 *  Created on: May 27, 2021
 *      Author: D.Wozniczka
 */

#include "screen.h"
#include "SSD1331.h"
#include "main.h"
#include "bitmaps.h"
#include "MAX30102.h"


#define OLED_WIDTH                      96
#define OLED_HEIGHT                     64


extern MAX30102_T hr_sensor;

void Screen_func(int screen_num){
	switch (screen_num){
		case 0:
			Display_fingerprint();
			break;
		case 1:
			Display_pulse();
			break;
		case 2:
		case 3:
			Display_heartpulse();
			break;
		default:
			break;
	}

}

void Display_Value(uint8_t pos_x, uint8_t pos_y, uint32_t value){

	ssd1331_fill_rect(pos_x, pos_y, 18, FONT_1206, BLACK);
	ssd1331_display_num(pos_x, pos_y, value, 3, FONT_1206, GREEN);
}

void Display_text(uint8_t chXpos, uint8_t chYpos,
		const uint8_t *pchString, uint8_t chSize, uint16_t hwColor) {
	if (chXpos >= OLED_WIDTH || chYpos >= OLED_HEIGHT) {
		return;
	}

	while (*pchString != '\0') {
		if (chXpos > (OLED_WIDTH - chSize / 2)) {
			chXpos = 0;
			chYpos += chSize;
			if (chYpos > (OLED_HEIGHT - chSize)) {
				chYpos = chXpos = 0;
				ssd1331_clear_screen(0x00);
			}
		}

		ssd1331_display_char(chXpos, chYpos, *pchString, chSize, hwColor);
		chXpos += chSize / 2;
		pchString++;
	}
}

void Display_fingerprint(void){

	  Display_text(0,48,"PUT YOUR FINGER", FONT_1206, WHITE);
	  ssd1331_draw_bitmap(24, 0, Bmp_fingerprint, 48, 48, WHITE);
}

void Display_pulse(void){

	Display_text(0,0,"Rate", FONT_1206, WHITE);
	Display_Value(0,12,hr_sensor.HeartRate);

	Display_text(0,36,"TEMP", FONT_1206, WHITE);
	Display_Value(0,48,hr_sensor.TmpValue);

	Display_text(70,36,"SAT", FONT_1206, WHITE);
	Display_Value(70,48,hr_sensor.Sp02Value);

	Display_heart();
}

void Display_heart(void){

	  ssd1331_draw_bitmap(67, 4, Bmp_heart, 24, 24, RED);
}

void Display_heartpulse(void){
	static uint8_t tick = 0;

	if(0 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_0, 64, 64, RED);
	}
	if(1 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_0, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_1, 64, 64, RED);
	}
	if(2 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_1, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_2, 64, 64, RED);
	}
	if(3 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_2, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_3, 64, 64, RED);
	}
	if(4 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_3, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_4, 64, 64, RED);
	}
	if(5 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_4, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_5, 64, 64, RED);
	}
	if(6 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_5, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_6, 64, 64, RED);
	}
	if(7 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_6, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_7, 64, 64, RED);
	}
	if(8 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_7, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_8, 64, 64, RED);
	}
	if(9 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_8, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_9, 64, 64, RED);
	}
	if(10 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_9, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_10, 64, 64, RED);
	}
	if(11 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_10, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_11, 64, 64, RED);
	}
	if(12 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_11, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_12, 64, 64, RED);
	}
	if(13 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_12, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_13, 64, 64, RED);
	}
	if(14 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_13, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_14, 64, 64, RED);
	}
	if(15 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_14, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_15, 64, 64, RED);
	}
	if(16 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_15, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_16, 64, 64, RED);
	}
	if(17 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_16, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_17, 64, 64, RED);
	}
	if(18 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_17, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_18, 64, 64, RED);
	}
	if(19 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_18, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_19, 64, 64, RED);
	}
	if(20 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_19, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_20, 64, 64, RED);
	}
	if(21 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_20, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_21, 64, 64, RED);
	}
	if(22 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_21, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_22, 64, 64, RED);
	}
	if(23 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_22, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_23, 64, 64, RED);
	}
	if(24 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_23, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_24, 64, 64, RED);
	}
	if(25 == tick){
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_24, 64, 64, BLACK);
		tick = -1;
	}
	tick++;
}

