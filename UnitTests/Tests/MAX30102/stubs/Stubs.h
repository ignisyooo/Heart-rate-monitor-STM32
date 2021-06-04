#ifndef STUBS_H
#define STUBS_H

#include "i2c.h"
#include "MAX30102.h"

I2C_HandleTypeDef hi2c1;

extern MAX30102_T hr_sensor;
extern MAX30102_STATE StateMachine;
extern FIFO_buffer fifo;

#endif /* STUBS_H */