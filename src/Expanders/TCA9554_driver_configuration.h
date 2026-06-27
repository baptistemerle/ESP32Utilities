#ifndef TCA9554_DRIVER_CONFIGURATION_H
#define TCA9554_DRIVER_CONFIGURATION_H

#include <stdint.h>

struct TCA9554Driver_Configuration
{
  uint8_t  i2cAddress;
  uint32_t i2cSpeedHz;
};

#endif // TCA9554_DRIVER_CONFIGURATION_H