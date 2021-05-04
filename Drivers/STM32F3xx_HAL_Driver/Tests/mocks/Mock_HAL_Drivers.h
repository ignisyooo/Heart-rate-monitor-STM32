#pragma once

#include <gmock/gmock.h>

extern "C"
{
#include "stm32f3xx_hal.h"
}

class Mock_HAL_Drivers
{
public:
    Mock_HAL_Drivers() { mock = this; }
    MOCK_METHOD7(HAL_I2C_Mem_Write, HAL_StatusTypeDef(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                                      uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout));
    MOCK_METHOD7(HAL_I2C_Mem_Read, HAL_StatusTypeDef(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                                     uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout));

    static Mock_HAL_Drivers *mock;
};