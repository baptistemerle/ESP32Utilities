#pragma once

#include <esp_err.h>
#include <soc/gpio_num.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"

namespace esp32utilities::expanders
{

class IExpander
{
protected:
  using GPIODirection = esp32utilities::interfaces::GPIO::Direction;
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;

public:
  virtual ~IExpander() = default;

  virtual esp_err_t setPinDirection(uint8_t pin, GPIODirection direction) = 0;
  virtual esp_err_t setPinLevel(uint8_t pin, GPIOLevel level) = 0;
  virtual esp_err_t readPinLevel(uint8_t pin, GPIOLevel* level) const = 0;
};

} // namespace esp32utilities::expanders
