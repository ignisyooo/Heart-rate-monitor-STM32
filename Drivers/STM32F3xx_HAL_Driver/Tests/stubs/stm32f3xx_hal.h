#ifndef __STM32F3xx_HAL_H
#define __STM32F3xx_HAL_H

#include <stdint.h>

#define HAL_MAX_DELAY 0xFFFFFFFFU

typedef enum
{
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03
} HAL_StatusTypeDef;

typedef struct __I2C_HandleTypeDef
{

} I2C_HandleTypeDef;

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif /* __STM32F3xx_HAL_H */