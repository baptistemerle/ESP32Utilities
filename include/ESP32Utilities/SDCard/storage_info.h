#pragma once

#include <cstdint>

namespace esp32utilities::sdcard
{

struct StorageInfo
{
  uint64_t totalBytes { 0 };
  uint64_t freeBytes  { 0 };
  uint64_t usedBytes  { 0 };
};

} // namespace esp32utilities::sdcard
