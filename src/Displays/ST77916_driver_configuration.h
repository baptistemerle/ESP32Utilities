#ifndef ST77916_DRIVER_CONFIGURATION_H
#define ST77916_DRIVER_CONFIGURATION_H

#include <driver/spi_common.h>
#include <soc/gpio_num.h>

struct ST77916Driver_Configuration
{
  spi_host_device_t spiHost;
  uint32_t          spiSpeedHz;
  gpio_num_t        spiPinChipSelect;
  gpio_num_t        pinReset;
  uint16_t          screenWidth;
  uint16_t          screenHeight;
};

#endif // ST77916_DRIVER_CONFIGURATION_H