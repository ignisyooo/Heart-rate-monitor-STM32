#include <gtest/gtest.h>
#include <stdint.h>
#include <stdio.h>

#include "Mock_HAL_Drivers.h"

using ::testing::_;
using ::testing::Assign;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;

extern "C"
{
#include "MAX30102.h"
#include "Stubs/Stubs.h"
}

#define BUFFER_SIZE 5U

namespace
{

    class MAX30102_test : public ::testing::Test
    {
    public:
        char dl_com[300];
        MAX30102_test()
        {
            mock_HAL_Drivers = new Mock_HAL_Drivers;
        }
        ~MAX30102_test()
        {
            delete mock_HAL_Drivers;
        }

        virtual void SetUp()
        {
            buffer = new uint8_t[BUFFER_SIZE];
            hi2c = (I2C_HandleTypeDef *)malloc(sizeof(I2C_HandleTypeDef));
        }
        virtual void TearDown()
        {
            delete[] buffer;
            free(hi2c);
        }

        uint8_t *buffer;
        static Mock_HAL_Drivers *mock_HAL_Drivers;
        I2C_HandleTypeDef *hi2c;
    };

    Mock_HAL_Drivers *MAX30102_test::mock_HAL_Drivers;

    TEST_F(MAX30102_test, Max30102_WriteReg_overwrite_buffer)
    {
        MAX30102_STATUS retVal;
        uint8_t mem_adress = 0;
        uint8_t test_buffer[BUFFER_SIZE];
        for (int idx = 0; idx < BUFFER_SIZE; ++idx)
        {
            test_buffer[idx] = 2 * idx;
            buffer[idx] = idx;
        }
        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(1)
            .WillRepeatedly(DoAll(
                Assign(buffer + 0, test_buffer[0]),
                Assign(buffer + 1, test_buffer[1]),
                Assign(buffer + 2, test_buffer[2]),
                Assign(buffer + 3, test_buffer[3]),
                Assign(buffer + 4, test_buffer[4]),
                Return(HAL_OK)));

        retVal = Max30102_WriteReg(mem_adress, test_buffer[0]);
        EXPECT_EQ(retVal, MAX30102_OK);
        for (int idx = 0; idx < BUFFER_SIZE; ++idx)
        {
            EXPECT_EQ(buffer[idx], test_buffer[idx]) << "Fail on idx: " << idx;
        }
    }

    TEST_F(MAX30102_test, Max30102_ReadReg_read_entire_buffer)
    {
        MAX30102_STATUS retVal;

        uint8_t test_buffer[BUFFER_SIZE];
        uint8_t mem_adress = 0;

        for (int idx = 0; idx < BUFFER_SIZE; ++idx)
        {
            buffer[idx] = idx;
            test_buffer[idx] = 0;
        }

        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Read(_, _, _, _, test_buffer, _, _))
            .WillOnce(DoAll(
                SetArrayArgument<4>(buffer + 0, buffer + BUFFER_SIZE),
                Return(HAL_OK)));

        retVal = Max30102_ReadReg(mem_adress, test_buffer);

        EXPECT_EQ(retVal, MAX30102_OK);

        for (int idx = 0; idx < BUFFER_SIZE; ++idx)
        {
            EXPECT_EQ(buffer[idx], idx);
            EXPECT_EQ(test_buffer[idx], idx);
        }
    }

    TEST_F(MAX30102_test, Max30102_SetIdleCurrent)
    {
        MAX30102_STATUS retVal;

        uint8_t IR_LED = MAX30102_RED_LED_CURRENT_HIGH;
        uint8_t RED_LED = MAX30102_IR_LED_CURRENT_LOW;
        memset(buffer, 0, sizeof(buffer));
        uint8_t idx = 0;

        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(2)
            .WillRepeatedly(DoAll(
                Assign(buffer + 0, IR_LED),
                Assign(buffer + 1, RED_LED),
                Return(HAL_OK)));

        retVal = Max30102_SetIdleCurrent(IR_LED, RED_LED);

        EXPECT_EQ(retVal, MAX30102_OK);

        EXPECT_EQ(buffer[0], MAX30102_RED_LED_CURRENT_HIGH);
        EXPECT_EQ(buffer[1], MAX30102_IR_LED_CURRENT_LOW);
    }

    TEST_F(MAX30102_test, Max30102_StateMachine_STATE_BEGIN)
    {
        StateMachine = MAX30102_STATE_BEGIN;
        hr_sensor.IsFingerOnScreen = 0;
        hr_sensor.HeartRate = 10;
        hr_sensor.Sp02Value = 12;
        fifo.CollectedSamples = 10;


        Max30102_StateMachine();

        EXPECT_EQ(hr_sensor.HeartRate, 0);
        EXPECT_EQ(hr_sensor.Sp02Value, 0);
        EXPECT_EQ(fifo.CollectedSamples, 10);

        hr_sensor.IsFingerOnScreen = 1;

        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(2);

        Max30102_StateMachine();

        EXPECT_EQ(fifo.CollectedSamples, 0);
        EXPECT_EQ(StateMachine, MAX30102_STATE_CALIBRATE);
    }

    TEST_F(MAX30102_test, Max30102_StateMachine_STATE_CALIBRATE)
    {
        StateMachine = MAX30102_STATE_CALIBRATE;
        hr_sensor.IsFingerOnScreen = 0;
        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(2);


        Max30102_StateMachine();

        EXPECT_EQ(StateMachine, MAX30102_STATE_BEGIN);

        StateMachine = MAX30102_STATE_CALIBRATE;
        hr_sensor.IsFingerOnScreen = 1;
        fifo.CollectedSamples = 0;

        Max30102_StateMachine();

        for (int i = 0; i <= MAX30102_BUFFER_LENGTH - MAX30102_SAMPLES_PER_SECOND; ++i)
        {
            fifo.CollectedSamples++;
            EXPECT_EQ(StateMachine, MAX30102_STATE_CALIBRATE);
        }

        Max30102_StateMachine();

        EXPECT_EQ(StateMachine, MAX30102_STATE_CALCULATE_HR);
    }

    TEST_F(MAX30102_test, Max30102_StateMachine_CALCULATE_HR)
    {
        StateMachine = MAX30102_STATE_CALCULATE_HR;
        hr_sensor.IsFingerOnScreen = 0;
        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(2);

        Max30102_StateMachine();

        EXPECT_EQ(StateMachine, MAX30102_STATE_BEGIN);

        StateMachine = MAX30102_STATE_CALCULATE_HR;
        hr_sensor.IsFingerOnScreen = 1;
        fifo.CollectedSamples = 10;

        Max30102_StateMachine();

        EXPECT_EQ(fifo.CollectedSamples, 0);
        EXPECT_EQ(StateMachine, MAX30102_STATE_COLLECT_NEXT_PORTION);
    }

    TEST_F(MAX30102_test, Max30102_StateMachine_COLLECT_NEXT_PORTION)
    {
        StateMachine = MAX30102_STATE_COLLECT_NEXT_PORTION;
        hr_sensor.IsFingerOnScreen = 0;
        EXPECT_CALL(*mock_HAL_Drivers, HAL_I2C_Mem_Write(_, _, _, _, _, _, _))
            .Times(2);


        Max30102_StateMachine();

        EXPECT_EQ(StateMachine, MAX30102_STATE_BEGIN);

        StateMachine = MAX30102_STATE_COLLECT_NEXT_PORTION;
        hr_sensor.IsFingerOnScreen = 1;
        fifo.CollectedSamples = 0;

        Max30102_StateMachine();

        for (int i = 0; i <= MAX30102_SAMPLES_PER_SECOND; ++i)
        {
            fifo.CollectedSamples++;
            EXPECT_EQ(StateMachine, MAX30102_STATE_COLLECT_NEXT_PORTION);
        }

        Max30102_StateMachine();

        EXPECT_EQ(StateMachine, MAX30102_STATE_CALCULATE_HR);
    }
}