#include <gtest/gtest.h>
#include <stdint.h>
#include <stdio.h>

#include "Mock_HAL_Drivers.h"

using ::testing::_;
using ::testing::Assign;
using ::testing::DoAll;
using ::testing::Return;

extern "C"
{
#include "MAX30102.h"
}

#define BUFFER_SIZE 5U

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

TEST_F(MAX30102_test, Max30102_WriteReg_test)
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