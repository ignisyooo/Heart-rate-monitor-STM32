/*
 * Interrupts.c
 *
 *  Created on: Apr 23, 2021
 *      Author: Rzeszutko
 */

#include "main.h"
#include "MAX30102.h"

extern MAX30102_T Max30102;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == INT_Pin) {
		Max30102_InterruptCallback(&Max30102);
	}
}
