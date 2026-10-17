#pragma once
#include "IO/Accelerometer/AccelerometerBase.h"

// A desktop has a fixed landscape orientation; mouse input emulates touches.
class AccelerometerLinux : public AccelerometerBase
{
public:
    AccelerometerLinux() {}
};
