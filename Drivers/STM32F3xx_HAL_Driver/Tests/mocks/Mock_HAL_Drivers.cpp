#include "Mock_HAL_Drivers.h"

Mock_HAL_Drivers *Mock_HAL_Drivers::mock;

extern "C"
{
    HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
    {
        return Mock_HAL_Drivers::mock->HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress,
                                                         MemAddSize, pData, Size, Timeout);
    }

    HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
    {
        return Mock_HAL_Drivers::mock->HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress,
                                                        MemAddSize, pData, Size, Timeout);
    }
}