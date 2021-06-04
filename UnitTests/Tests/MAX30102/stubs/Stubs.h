#ifndef STUBS_H
#define STUBS_H

#include "i2c.h"
#include "MAX30102.h"
#include "LIB_Config.h"
#include "Fonts.h"

#include "stm32f3xx_hal.h"

I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi2;

extern MAX30102_T hr_sensor;
extern MAX30102_STATE StateMachine;
extern FIFO_buffer fifo;

extern const uint8_t c_chFont1206[95][12];
extern const uint8_t c_chFont1608[95][16];
extern const uint8_t c_chFont1612[11][32];
extern const uint8_t c_chFont3216[11][64];
extern const uint8_t c_chBmp4016[96];
extern const uint8_t c_chSingal816[16];
extern const uint8_t c_chMsg816[16];
extern const uint8_t c_chBluetooth88[8];
extern const uint8_t c_chBat816[16];
extern const uint8_t c_chGPRS88[8];
extern const uint8_t c_chAlarm88[8];

void ssd1331_init(void);
void Screen_func(int screen_num);

#endif /* STUBS_H */