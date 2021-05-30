#include "screen.h"
#include "SSD1331.h"
#include "main.h"
#include "bitmaps.h"
/*
 * screen.c
 *
 *  Created on: May 27, 2021
 *      Author: D.Wozniczka
 */
#define OLED_WIDTH                      96
#define OLED_HEIGHT                     64




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

void Display_pulse(uint32_t HeartRate, uint32_t Sat, uint32_t Temp){

	Display_text(0,0,"Rate", FONT_1206, WHITE);
	Display_Value(0,12,HeartRate);

	Display_text(0,36,"TEMP", FONT_1206, WHITE);
	Display_Value(0,48,Temp);

	Display_text(70,36,"SAT", FONT_1206, WHITE);
	Display_Value(70,48,Sat);

	Display_heart();
}

void Display_heart(void){

	  ssd1331_draw_bitmap(67, 4, Bmp_heart, 24, 24, RED);
}

void Display_heartpulse(int sec){

	HAL_Delay(1000);
	for(int i = 0; i < sec; i++){

		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_0, 64, 64, RED);

		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_0, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_1, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_1, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_2, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_2, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_3, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_3, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_4, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_4, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_5, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_5, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_6, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_6, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_7, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_7, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_8, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_8, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_9, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_9, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_10, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_10, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_11, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_11, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_12, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_12, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_13, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_13, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_14, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_14, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_15, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_15, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_16, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_16, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_17, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_17, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_18, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_18, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_19, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_19, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_20, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_20, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_21, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_21, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_22, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_22, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_23, 64, 64, RED);
		HAL_Delay(00);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_23, 64, 64, BLACK);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_24, 64, 64, RED);
		HAL_Delay(10);
		ssd1331_draw_bitmap(0, 0, Bmp_heartpulse_frame_24, 64, 64, BLACK);
	}
}

