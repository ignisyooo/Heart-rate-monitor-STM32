/*
 * MAX30102.h
 *
 *  Created on: Apr 23, 2021
 *      Author: Rzeszutko
 */


#ifndef MAX30102_H_
#define MAX30102_H_



/****************************************************************************
 * Private defines												*
 ***************************************************************************/
#define I2C_TIMEOUT 1U

/* Registers */
#define MAX30102_ADDRESS 			0xAEU

#define REG_INTR_ENABLE_1 			0x02U
#define REG_INTR_ENABLE_2 			0x03U

#define REG_FIFO_WR_PTR 			0x04U
#define REG_FIFO_RD_PTR 			0x06U
#define REG_OVF_COUNTER 			0x05U
#define REG_FIFO_DATA 				0x07U

#define REG_FIFO_CONFIG 			0x08U
#define REG_MODE_CONFIG 			0x09U
#define REG_SPO2_CONFIG 			0x0AU
#define REG_TEMP_CONFIG 			0x21U

#define REG_LED1_PA 				0x0CU
#define REG_LED2_PA 				0x0DU

#define MAX30102_IR_LED_CURRENT_LOW		0x01U
#define MAX30102_RED_LED_CURRENT_LOW	0x00U
#define MAX30102_IR_LED_CURRENT_HIGH	0x24U
#define MAX30102_RED_LED_CURRENT_HIGH	0x24U

#define	INT_A_FULL_BIT				0x07U
#define	INT_PPG_RDY_BIT				0x06U
#define	INT_DIE_TEMP_RDY_BIT		0x01U

#define REG_TEMP_INTR 				0x1FU
#define REG_TEMP_FRAC 				0x20U

#define REG_INTR_STATUS_1 			0x00U
#define REG_INTR_STATUS_2 			0x01U

#define FIFO_ALMOST_FULL_SAMPLES 	0x11U


#define MAX30102_MEASUREMENT_SECONDS 	5U
#define MAX30102_SAMPLES_PER_SECOND		100U

#define MAX30102_IR_VALUE_FINGER_OUT_SENSOR 50000U
#define MAX30102_IR_VALUE_FINGER_ON_SENSOR   1600U

#define MAX30102_BUFFER_LENGTH	((MAX30102_MEASUREMENT_SECONDS+1)*MAX30102_SAMPLES_PER_SECOND)

#define GET_SIZE_OF_ARRAY(tab) ((sizeof(tab))/(sizeof(tab[0])))

/****************************************************************************
 * Private structure														*
 ***************************************************************************/
typedef struct
{
	uint8_t mem_adress;
	uint8_t Value;
}MAX30102_config_T;

typedef struct
{
	volatile uint32_t IrBuffer[MAX30102_BUFFER_LENGTH];	 //IR LED sensor data
	volatile uint32_t RedBuffer[MAX30102_BUFFER_LENGTH]; //Red LED sensor data
	volatile uint32_t BufferHead;
	volatile uint32_t BufferTail;
	volatile uint32_t CollectedSamples;
}FIFO_buffer;

typedef struct
{
	I2C_HandleTypeDef *i2c;

	uint8_t IsFingerOnScreen;

	FIFO_buffer FIFO;

	struct{
		uint32_t  Sp02Value;
		uint32_t HeartRate;
		float TmpValue;
	};

}MAX30102_T;


/****************************************************************************
 * Private enums															*
 ***************************************************************************/
typedef enum{
	MAX30102_ERROR 	= 0,
	MAX30102_OK 	= 1
} MAX30102_STATUS;

typedef enum
{
	MAX30102_STATE_BEGIN,
	MAX30102_STATE_CALIBRATE,
	MAX30102_STATE_CALCULATE_HR,
	MAX30102_STATE_COLLECT_NEXT_PORTION
} MAX30102_STATE;

/****************************************************************************
 * Private functions														*
 ***************************************************************************/

MAX30102_STATUS Max30102_Reset(void);
MAX30102_STATUS Max30102_Init(MAX30102_T *self, I2C_HandleTypeDef *i2c);
MAX30102_STATUS Max30102_SetIdleCurrent(uint8_t led1, uint8_t led2);

MAX30102_STATUS Max30102_ReadFifo(volatile uint32_t * const pun_red_led, volatile uint32_t *const pun_ir_led);
MAX30102_STATUS Max30102_WriteReg(uint8_t mem_adress, uint8_t data);
MAX30102_STATUS Max30102_ReadReg(uint8_t mem_adress, uint8_t *data);
MAX30102_STATUS Max30102_ReadInterruptStatus(uint8_t * const status);

void Max30102_FIFO_data_sort(FIFO_buffer * fifo, uint8_t *isFingerOn);
void Max30102_InterruptCallback(MAX30102_T *self);
void Max30102_StateMachine(MAX30102_T *self);





#endif /* MAX30102_H_ */
