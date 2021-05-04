#include "Mock_MAX30102.h"

Mock_MAX30102 *Mock_MAX30102::mock;

extern "C"
{
    MAX30102_STATUS Max30102_Init(MAX30102_T *self, I2C_HandleTypeDef *i2c)
    {
        return Mock_MAX30102::mock->Max30102_Init(self, i2c);
    }
    /*
    MAX30102_STATUS Max30102_WriteReg(uint8_t mem_adress, uint8_t data)
    {
        return Mock_MAX30102::mock->Max30102_WriteReg(mem_adress, data);
    }
    */
}