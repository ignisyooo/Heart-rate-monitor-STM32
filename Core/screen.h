/*
 * screen.h
 *
 *  Created on: May 27, 2021
 *      Author: Himrodel
 */
#include "stdint.h"

#ifndef SCREEN_H_
#define SCREEN_H_

void Display_Value(uint8_t pos_x, uint8_t pos_y, uint32_t value);
void Display_text(uint8_t chXpos, uint8_t chYpos,const uint8_t *pchString, uint8_t chSize, uint16_t hwColor);
void Display_heart(void);
void Display_pulse(uint32_t, uint32_t, uint32_t);
void Display_fingerprint(void);

#endif /* SCREEN_H_ */
