#pragma once

#include <driver/spi_common.h>
#include <soc/gpio_num.h>

namespace esp32utilities::displays::ST77916
{

struct Configuration
{
  spi_host_device_t spiHost;
  uint32_t          spiSpeedHz;
  gpio_num_t        spiPinChipSelect;
  gpio_num_t        pinReset;
  uint16_t          screenWidth;
  uint16_t          screenHeight;
};

} // namespace esp32utilities::displays::ST77916
