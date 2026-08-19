#pragma once

#include <driver/spi_common.h>
#include <esp_lcd_panel_rgb.h>
#include <soc/gpio_num.h>

namespace esp32utilities::displays::ST7701
{

struct Configuration
{
  uint16_t screenWidth;
  uint16_t screenHeight;

  spi_host_device_t spiHost;
  uint32_t          spiSpeedHz;

  gpio_num_t pinPCLK;
  gpio_num_t pinHSYNC;
  gpio_num_t pinVSYNC;
  gpio_num_t pinDE;
  gpio_num_t dataPins[16];

  esp_lcd_rgb_timing_t timings;
};

} // namespace esp32utilities::displays::ST7701
