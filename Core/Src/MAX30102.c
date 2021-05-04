#include "main.h"
#include "i2c.h"

#include "MAX30102.h"

/* Array with addresses and values for configuration of registers */
static MAX30102_config_T config_data[] = {
	{
		.mem_adress = REG_INTR_ENABLE_2, .Value = 0x02 /* Enable interrupt from temperature conversion end */
	},
	{.mem_adress = REG_INTR_ENABLE_1, .Value = 0x48},
	/* FIFO pointers setting */
	{
		.mem_adress = REG_FIFO_WR_PTR, .Value = 0x00},
	{.mem_adress = REG_FIFO_RD_PTR, .Value = 0x00},
	{.mem_adress = REG_OVF_COUNTER, .Value = 0x00},
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
	}};

static MAX30102_STATE StateMachine;

MAX30102_T Max30102;

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

MAX30102_STATUS Max30102_WriteReg(uint8_t mem_adress, uint8_t data)
{
	if (HAL_OK != HAL_I2C_Mem_Write(Max30102.i2c, MAX30102_ADDRESS, mem_adress, sizeof(mem_adress), &data, sizeof(data), I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	return MAX30102_OK;
}

MAX30102_STATUS Max30102_ReadReg(uint8_t mem_adress, uint8_t *data)
{
	if (HAL_OK != HAL_I2C_Mem_Read(Max30102.i2c, MAX30102_ADDRESS, mem_adress, sizeof(mem_adress), data, sizeof(data), I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}
	return MAX30102_OK;
}

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

MAX30102_STATUS Max30102_ReadFifo(volatile uint32_t *const pun_red_led, volatile uint32_t *const pun_ir_led)
{
	*pun_red_led = 0;
	*pun_ir_led = 0;
	uint8_t i2c_fifo_data[6];

	if (HAL_OK != HAL_I2C_Mem_Read(Max30102.i2c, MAX30102_ADDRESS, REG_FIFO_DATA, sizeof(i2c_fifo_data[0]), i2c_fifo_data, GET_SIZE_OF_ARRAY(i2c_fifo_data), I2C_TIMEOUT))
	{
		return MAX30102_ERROR;
	}

	*pun_ir_led = (i2c_fifo_data[0] << 16 | i2c_fifo_data[1] << 8 | i2c_fifo_data[2]) & 0x3FFFFU;
	*pun_red_led = (i2c_fifo_data[3] << 16 | i2c_fifo_data[4] << 8 | i2c_fifo_data[5]) & 0x3FFFFU;

	return MAX30102_OK;
}

MAX30102_STATUS Max30102_ReadInterruptStatus(uint8_t *const status)
{
	*status = 0;
	uint8_t tmp;
	if (MAX30102_OK != Max30102_ReadReg(REG_INTR_STATUS_1, &tmp))
	{
		return MAX30102_ERROR;
	}
	*status |= tmp & 0xE1; // 3 highest bits
	if (MAX30102_OK != Max30102_ReadReg(REG_INTR_STATUS_2, &tmp))
	{
		return MAX30102_ERROR;
	}
	*status |= tmp & 0x02;
	return MAX30102_OK;
}

void Max30102_FIFO_data_sort(FIFO_buffer *fifo, uint8_t *isFingerOn)
{
	while (MAX30102_OK != Max30102_ReadFifo(&(fifo->RedBuffer[fifo->BufferHead]), &(fifo->IrBuffer[fifo->BufferHead])))
		;
	if (*isFingerOn)
	{
		if (fifo->IrBuffer[fifo->BufferHead] < MAX30102_IR_VALUE_FINGER_OUT_SENSOR)
			*isFingerOn = 0;
	}
	else
	{
		if (fifo->IrBuffer[fifo->BufferHead] > MAX30102_IR_VALUE_FINGER_ON_SENSOR)
			*isFingerOn = 1;
	}
	fifo->BufferHead = (fifo->BufferHead + 1) % MAX30102_BUFFER_LENGTH;
	fifo->CollectedSamples++;
}

void Max30102_InterruptCallback(MAX30102_T *self)
{
	uint8_t interrupt_status = 0;
	uint8_t temp_int = 0;
	uint8_t temp_fraction = 0;
	while (MAX30102_OK != Max30102_ReadInterruptStatus(&interrupt_status))
		;

	/* Almost Full FIFO Interrupt handle */
	if (interrupt_status & (1 << INT_A_FULL_BIT))
	{
		for (uint8_t idx = 0; idx < FIFO_ALMOST_FULL_SAMPLES; ++idx)
		{
			Max30102_FIFO_data_sort(&(self->FIFO), &(self->IsFingerOnScreen));
		}
	}

	// New FIFO Data Ready Interrupt handle
	if (interrupt_status & (1 << INT_PPG_RDY_BIT))
	{
		Max30102_FIFO_data_sort(&(self->FIFO), &(self->IsFingerOnScreen));
	}

	if (interrupt_status & (1 << INT_DIE_TEMP_RDY_BIT))
	{
		Max30102_ReadReg(REG_TEMP_INTR, &temp_int);
		Max30102_ReadReg(REG_TEMP_FRAC, &temp_fraction);
		self->TmpValue = (float)temp_int + ((float)temp_fraction * 0.0625);
	}
}

void Max30102_StateMachine(MAX30102_T *self)
{
	switch (StateMachine)
	{
	case MAX30102_STATE_BEGIN:
		self->HeartRate = 0;
		self->Sp02Value = 0;
		if (self->IsFingerOnScreen)
		{
			self->FIFO.CollectedSamples = 0;
			self->FIFO.BufferTail = self->FIFO.BufferHead;
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_HIGH, MAX30102_IR_LED_CURRENT_HIGH);
			StateMachine = MAX30102_STATE_CALIBRATE;
		}
		break;

	case MAX30102_STATE_CALIBRATE:
		if (self->IsFingerOnScreen)
		{
			if (self->FIFO.CollectedSamples > (MAX30102_BUFFER_LENGTH - MAX30102_SAMPLES_PER_SECOND))
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
		if (self->IsFingerOnScreen)
		{
			maxim_heart_rate_and_oxygen_saturation(self->FIFO.IrBuffer, self->FIFO.RedBuffer, MAX30102_BUFFER_LENGTH - MAX30102_SAMPLES_PER_SECOND, self->FIFO.BufferTail, &(self->Sp02Value), &(self->Sp02IsValid), &(self->HeartRate), &(self->IsHrValid));
			self->FIFO.CollectedSamples = 0;
			self->FIFO.BufferTail = (self->FIFO.BufferTail + MAX30102_SAMPLES_PER_SECOND) % MAX30102_BUFFER_LENGTH;
			StateMachine = MAX30102_STATE_COLLECT_NEXT_PORTION;
		}
		else
		{
			Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_LOW);
			StateMachine = MAX30102_STATE_BEGIN;
		}
		break;

	case MAX30102_STATE_COLLECT_NEXT_PORTION:
		if (self->IsFingerOnScreen)
		{
			if (self->FIFO.CollectedSamples > MAX30102_SAMPLES_PER_SECOND)
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

MAX30102_STATUS Max30102_Init(MAX30102_T *self, I2C_HandleTypeDef *i2c)
{
	self->i2c = &hi2c1;
	if (MAX30102_OK != Max30102_Reset()) /*resets the MAX30102 */
	{
		return MAX30102_ERROR;
	}

	for (int idx = 0; idx <= GET_SIZE_OF_ARRAY(config_data); ++idx)
	{
		if (MAX30102_OK != Max30102_WriteReg(config_data[idx].mem_adress, config_data[idx].Value))
		{
			return MAX30102_ERROR;
		}
	}
	Max30102_SetIdleCurrent(MAX30102_RED_LED_CURRENT_LOW, MAX30102_IR_LED_CURRENT_LOW);
	StateMachine = MAX30102_STATE_BEGIN;
	return MAX30102_OK;
}
