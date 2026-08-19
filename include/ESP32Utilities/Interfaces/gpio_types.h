#pragma once

#include <cstdint>

namespace esp32utilities::interfaces::GPIO
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

} // namespace esp32utilities::interfaces::GPIO
