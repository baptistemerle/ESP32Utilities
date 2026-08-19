#pragma once

#include <cstdint>

namespace esp32utilities::expanders::TCA9554
{

struct Configuration
{
  uint8_t  i2cAddress;
  uint32_t i2cSpeedHz;
};

} // namespace esp32utilities::expanders::TCA9554
