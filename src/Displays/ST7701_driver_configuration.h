#ifndef ST7701_DRIVER_CONFIGURATION_H
#define ST7701_DRIVER_CONFIGURATION_H

#include <esp_lcd_panel_rgb.h>

#include <driver/spi_common.h>
#include <soc/gpio_num.h>

struct ST7701Driver_Configuration
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

#endif // ST7701_DRIVER_CONFIGURATION_H