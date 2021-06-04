#pragma once

#include <gmock/gmock.h>
#include "Mock_HAL_Drivers.h"

extern "C"
{
#include "MAX30102.h"
#include "screen.h"
}

class Mock_MAX30102
{
public:
    Mock_MAX30102() { mock = this; }

    static Mock_MAX30102 *mock;
};