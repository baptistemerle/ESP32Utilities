#pragma once

#include <functional>

#include <freertos/FreeRTOS.h>

#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_ops.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"
#include "ESP32Utilities/Displays/idisplay_driver.h"

#include "configuration.h"

namespace esp32utilities::displays::ST7701
{

class Driver : public IDisplayDriver
{
protected:
  using GPIODirection = esp32utilities::interfaces::GPIO::Direction;
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;
  using PinControlCallback = std::function<void(GPIOLevel level)>;

public:
  Driver(const Configuration&     configuration,
               PinControlCallback setCsCallback,
               PinControlCallback setResetCallback);
  virtual ~Driver();

  void init(DisplayTxDoneCallback callback, void* callbackArg) override;
  void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawBuffer) override;

  uint32_t width() const override;
  uint32_t height() const override;

  DisplayRenderMode preferredRenderMode() const override;
  void* getFrameBuffer(uint8_t index) const override;

private:
  void initST7701SRegisters();
  void reset();

  esp_err_t writeCommand9Bit(uint8_t cmd);
  esp_err_t writeData9Bit(uint8_t data);

private:
  const Configuration m_configuration;

  PinControlCallback m_setCsCallback;
  PinControlCallback m_setResetCallback;

  esp_lcd_panel_handle_t m_panelHandle     { nullptr };
  void*                  m_framebuffers[2] { nullptr, nullptr };

  spi_device_handle_t m_spiHandle { nullptr };

  TaskHandle_t m_flushTaskHandle { nullptr };
};

} // namespace esp32utilities::displays::ST7701
