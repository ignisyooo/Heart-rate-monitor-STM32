#pragma once

#include <gmock/gmock.h>
#include "Mock_HAL_Drivers.h"

extern "C"
{
#include "MAX30102.h"
}

class Mock_MAX30102
{
public:
    Mock_MAX30102() { mock = this; }
    MOCK_METHOD2(Max30102_Init, MAX30102_STATUS(MAX30102_T *self, I2C_HandleTypeDef *i2c));
    //MOCK_METHOD2(Max30102_WriteReg, MAX30102_STATUS(uint8_t mem_adress, uint8_t data));

    static Mock_MAX30102 *mock;
};