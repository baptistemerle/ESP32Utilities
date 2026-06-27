#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

#include <stdint.h>

namespace GPIO
{
  enum class Direction : uint8_t
  {
    Input,
    Output,
  };

  enum class Level : uint8_t
  {
    Low,
    High,
  };
}

#endif // GPIO_TYPES_H