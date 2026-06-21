#ifndef GC9A01_DRIVER_CONFIGURATION_H
#define GC9A01_DRIVER_CONFIGURATION_H

#include <driver/spi_common.h>
#include <soc/gpio_num.h>

struct GC9A01Driver_Configuration
{
  spi_host_device_t spiHost;
  uint32_t          spiFrequencyWrite;
  gpio_num_t        spiPinChipSelect;
  gpio_num_t        pinReset;
  gpio_num_t        pinDataCommand;
  uint16_t          screenWidth;
  uint16_t          screenHeight;
};

#endif // GC9A01_DRIVER_CONFIGURATION_H