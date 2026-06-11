#ifndef GC9A01_DRIVER_CONFIGURATION_H
#define GC9A01_DRIVER_CONFIGURATION_H

#include <driver/spi_common.h>

struct GC9A01Driver_Configuration
{
  spi_host_device_t spiHost;
  uint32_t          spiFrequencyWrite;
  int16_t           spiPinClock;
  int16_t           spiPinMOSI;
  int16_t           spiPinChipSelect;
  int16_t           pinReset;
  int16_t           pinDataCommand;
  uint16_t          screenWidth;
  uint16_t          screenHeight;
};

#endif // GC9A01_DRIVER_CONFIGURATION_H