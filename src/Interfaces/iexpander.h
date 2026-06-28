#ifndef IEXPANDER_H
#define IEXPANDER_H

#include "gpio_types.h"

#include <esp_err.h>
#include <soc/gpio_num.h>

class IExpander
{
public:
  virtual ~IExpander() = default;

  virtual esp_err_t setPinDirection(uint8_t pin, GPIO::Direction direction) = 0;
  virtual esp_err_t setPinLevel(uint8_t pin, GPIO::Level level) = 0;
  virtual esp_err_t readPinLevel(uint8_t pin, GPIO::Level* level) const = 0;
};

#endif // IEXPANDER_H