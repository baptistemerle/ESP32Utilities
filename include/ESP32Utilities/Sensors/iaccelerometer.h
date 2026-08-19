#pragma once

#include <esp_err.h>

namespace esp32utilities::sensors
{

class IAccelerometer
{
public:
  virtual ~IAccelerometer() = default;

  virtual esp_err_t getValues(float& x, float& y, float& z) const = 0;
};

} //namespace esp32utilities::sensors
