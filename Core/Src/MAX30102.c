#include "main.h"
#include "i2c.h"
#include "string.h"

#include "MAX30102.h"
#include "algorithm.h"

#include "Unit_tests.h"

/****************************************************************************
 * Static variables															*
 ***************************************************************************/

volatile FIFO_buffer fifo;
MAX30102_T hr_sensor;
MAX30102_STATE StateMachine;

/* Array with addresses and values for configuration of registers */
static MAX30102_config_T config_data[] = {
	{
			.mem_adress = REG_INTR_ENABLE_2, .Value = 0x02
	},
	{
			.mem_adress = REG_INTR_ENABLE_1, .Value = 0xc0
	},
	/* FIFO pointers setting */
	{
		.mem_adress = REG_FIFO_WR_PTR, .Value = 0x00
	},
	{
			.mem_adress = REG_FIFO_RD_PTR, .Value = 0x00
	},
	{
			.mem_adress = REG_OVF_COUNTER, .Value = 0x00
	},
	/* FIFO config settings - sample average = 4, fifo rollover = enable(?), fifo almost full value = 17 */
	{
		.mem_adress = REG_FIFO_CONFIG, .Value = 0x1F},
	/* Set mode. HR mode - 0b010; SpO2 mode - 0b011; Multi-LED mode - 0b111 */
	{
		.mem_adress = REG_MODE_CONFIG, .Value = 0x03},
	/* SpO2 mode settings. ADC range (15,63 / 4096) - 01; Sample frequence - 100 - 001; LED pulse width - 411us - 11 */
	{
		.mem_adress = REG_SPO2_CONFIG,
		.Value = 0x27,
	}
};

/****************************************************************************
 * Read/Write functions														*
 ***************************************************************************/
MAX30102_STATUS Max30102_WriteReg(uint8_t mem_addr, uint8_t data)
{
	if (HAL_OK != HAL_I2C_Mem_Write(hr_sensor.i2c, MAX30102_ADDRESS, mem_addr, 1, &data, 1, I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	return MAX30102_OK;
}

MAX30102_STATUS Max30102_ReadReg(uint8_t mem_addr, uint8_t *data)
{
	if (HAL_OK != HAL_I2C_Mem_Read(hr_sensor.i2c, MAX30102_ADDRESS, mem_addr, 1, data, 1, I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	return MAX30102_OK;
}

MAX30102_STATUS Max30102_SetMode(uint8_t mem_addr, uint8_t data)
{
	uint8_t tmp = 0;
	if (HAL_OK != HAL_I2C_Mem_Read(hr_sensor.i2c, MAX30102_ADDRESS, mem_addr, 1, &tmp, 1, I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	tmp |= data;
	if (HAL_OK != HAL_I2C_Mem_Write(hr_sensor.i2c, MAX30102_ADDRESS, mem_addr, 1, &tmp, 1, I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	return MAX30102_OK;
}

MAX30102_STATUS Max30102_ReadFifo(volatile uint32_t *red_led_ptr, volatile uint32_t *ir_led_ptr)
{
	*red_led_ptr = 0;
	*ir_led_ptr = 0;
	uint8_t i2c_fifo_data[6];

	if (HAL_OK != HAL_I2C_Mem_Read(hr_sensor.i2c, MAX30102_ADDRESS, REG_FIFO_DATA, 1, i2c_fifo_data, 6, I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}

	*red_led_ptr += ((((uint32_t)i2c_fifo_data[0]) << 16) | (((uint32_t)i2c_fifo_data[1]) << 8) | ((uint32_t)i2c_fifo_data[2])) & 0x3FFFFU;
	*ir_led_ptr += ((((uint32_t)i2c_fifo_data[3]) << 16) | (((uint32_t)i2c_fifo_data[4]) << 8) | ((uint32_t)i2c_fifo_data[5])) & 0x3FFFFU;

	return MAX30102_OK;
}

void Max30102_DataAnalysis(void)
{
	while (MAX30102_OK != Max30102_ReadFifo((fifo.RedBuffer + fifo.BufferHead), (fifo.IrBuffer + fifo.BufferHead)))
		;
	if (hr_sensor.IsFingerOnScreen)
	{
		if (fifo.IrBuffer[fifo.BufferHead] < MAX30102_IR_VALUE_FINGER_OUT_SENSOR)
			hr_sensor.IsFingerOnScreen = 0;
	}
	else
	{
		if (fifo.IrBuffer[fifo.BufferHead] > MAX30102_IR_VALUE_FINGER_ON_SENSOR)
			hr_sensor.IsFingerOnScreen = 1;
	}
	fifo.BufferHead = (fifo.BufferHead + 1) % MAX30102_BUFFER_LENGTH;
	fifo.CollectedSamples++;
}

MAX30102_STATUS Max30102_ReadTemperature(void)
{
	uint8_t temp_int;
	uint8_t temp_fraction;

	if (MAX30102_OK != Max30102_ReadReg(REG_TEMP_INTR, &temp_int))
	{
		return MAX30102_ERROR;
	}
	if (MAX30102_OK != Max30102_ReadReg(REG_TEMP_FRAC, &temp_fraction))
	{
		return MAX30102_ERROR;
	}

	hr_sensor.TmpValue = (float)temp_int + (((float)temp_fraction) * 0.0625);

	return MAX30102_OK;
}

/****************************************************************************
 * Interrupts handling														*
 ***************************************************************************/
MAX30102_STATUS Max30102_ReadInterruptStatus(uint8_t *Status)
{
	uint8_t tmp;
	*Status = 0;

	if (MAX30102_OK != Max30102_ReadReg(REG_INTR_STATUS_1, &tmp))
	{
		return MAX30102_ERROR;
	}

	*Status |= tmp & 0xE1; // 3 highest bits

	if (MAX30102_OK != Max30102_ReadReg(REG_INTR_STATUS_2, &tmp))
	{
		return MAX30102_ERROR;
	}
	*Status |= tmp & 0x02; // temperature interrupt

	return MAX30102_OK;
}

void Max30102_InterruptCallback(void)
{
	uint8_t Status;
	while (MAX30102_OK != Max30102_ReadInterruptStatus(&Status))
		;

	// Almost Full FIFO Interrupt handle
	if (Status & (1 << INT_A_FULL_BIT))
	{
		for (uint8_t i = 0; i < MAX30102_FIFO_ALMOST_FULL_SAMPLES; i++)
		{
			Max30102_DataAnalysis();
		}
	}

	// New FIFO Data Ready Interrupt handle
	if (Status & (1 << INT_PPG_RDY_BIT))
	{
		Max30102_DataAnalysis();
	}

	if (Status & (1 << INT_DIE_TEMP_RDY_BIT))
	{
		Max30102_ReadTemperature();
	}
}

/****************************************************************************
 * Reset configuration														*
 ***************************************************************************/
MAX30102_STATUS Max30102_Reset(void)
{
	uint8_t tmp = 0xFF;
	if (MAX30102_OK != Max30102_WriteReg(REG_MODE_CONFIG, 0x40))
	{
		return MAX30102_ERROR;
	}
	do
	{
		if (MAX30102_OK != Max30102_ReadReg(REG_MODE_CONFIG, &tmp))
		{
			return MAX30102_ERROR;
		}
	} while (tmp & (1 << 6));

	return MAX30102_OK;
}

/****************************************************************************
 * StateMachine																*
 ***************************************************************************/
void Max30102_StateMachine(void)
{
	switch (StateMachine)
	{
	case MAX30102_STATE_BEGIN:
		hr_sensor.HeartRate = 0;
		hr_sensor.Sp02Value = 0;
		if (hr_sensor.IsFingerOnScreen)
		{
			fifo.CollectedSamples = 0;
			fifo.BufferTail = fifo.BufferHead;
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_HIGH, MAX30102_IR_LED_CURRENT_HIGH);
			StateMachine = MAX30102_STATE_CALIBRATE;
		}
		break;

	case MAX30102_STATE_CALIBRATE:
		if (hr_sensor.IsFingerOnScreen)
		{
			if (fifo.CollectedSamples > (MAX30102_BUFFER_LENGTH - MAX30102_SAMPLES_PER_SECOND))
			{
				StateMachine = MAX30102_STATE_CALCULATE_HR;
			}
		}
		else
		{
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_LOW);
			StateMachine = MAX30102_STATE_BEGIN;
		}
		break;

	case MAX30102_STATE_CALCULATE_HR:
		if (hr_sensor.IsFingerOnScreen)
		{
			maxim_heart_rate_and_oxygen_saturation(fifo.RedBuffer, fifo.IrBuffer, MAX30102_BUFFER_LENGTH - MAX30102_SAMPLES_PER_SECOND, fifo.BufferTail, &hr_sensor.Sp02Value, &hr_sensor.Sp02IsValid, &hr_sensor.HeartRate, &hr_sensor.IsHrValid);
			fifo.BufferTail = (fifo.BufferTail + MAX30102_SAMPLES_PER_SECOND) % MAX30102_BUFFER_LENGTH;
			fifo.CollectedSamples = 0;
			StateMachine = MAX30102_STATE_COLLECT_NEXT_PORTION;
		}
		else
		{
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_LOW);
			StateMachine = MAX30102_STATE_BEGIN;
		}
		break;

	case MAX30102_STATE_COLLECT_NEXT_PORTION:
		if (hr_sensor.IsFingerOnScreen)
		{
			if (fifo.CollectedSamples > MAX30102_SAMPLES_PER_SECOND)
			{
				StateMachine = MAX30102_STATE_CALCULATE_HR;
			}
		}
		else
		{
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_LOW);
			StateMachine = MAX30102_STATE_BEGIN;
		}
		break;
	}
}

/****************************************************************************
 * LEDs current																*
 ***************************************************************************/
MAX30102_STATUS Max30102_SetIdleCurrent(uint8_t led1, uint8_t led2)
{
	if (MAX30102_OK != Max30102_WriteReg(REG_LED1_PA, led1))
	{
		return MAX30102_ERROR;
	}
	if (MAX30102_OK != Max30102_WriteReg(REG_LED2_PA, led2))
	{
		return MAX30102_ERROR;
	}

	return MAX30102_OK;
}

/****************************************************************************
 * Init function															*
 ***************************************************************************/
MAX30102_STATUS Max30102_Init(I2C_HandleTypeDef *i2c)
{
	hr_sensor.i2c = i2c;
	memset(&fifo, 0, sizeof(fifo));
	if (MAX30102_OK != Max30102_Reset())
	{
		return MAX30102_ERROR;
	}

	for (int idx = 0; idx <= GET_SIZE_OF_ARRAY(config_data); ++idx)
	{
		if (MAX30102_OK != Max30102_SetMode(config_data[idx].mem_adress, config_data[idx].Value))
		{
			return MAX30102_ERROR;
		}
	}
	Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_HIGH);
	StateMachine = MAX30102_STATE_BEGIN;
	return MAX30102_OK;
}
