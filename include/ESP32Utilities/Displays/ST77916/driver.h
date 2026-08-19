#pragma once

#include <functional>

#include <esp_lcd_panel_io.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"
#include "ESP32Utilities/Displays/idisplay_driver.h"

#include "configuration.h"

namespace esp32utilities::displays::ST77916
{

class Driver : public IDisplayDriver
{
protected:
  using GPIODirection = esp32utilities::interfaces::GPIO::Direction;
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;
  using PinControlCallback = std::function<void(GPIOLevel level)>;

public:
  Driver(const Configuration&     configuration,
               PinControlCallback setResetCallback);
  virtual ~Driver();

  void init(DisplayTxDoneCallback callback, void* callbackArg) override;
  void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData) override;

  uint32_t width() const override;
  uint32_t height() const override;
  DisplayRenderMode preferredRenderMode() const override;
  bool requiresByteSwap() const override;

private:
  enum class ScreenRevision
  {
    Case1, // Standard
    Case2  // Specific, needs st77916_factoryCommands
  };

private:
  void reset();
  ScreenRevision probeScreenRevision() const;

private:
  const Configuration m_configuration;

  PinControlCallback m_setResetCallback;

  esp_lcd_panel_io_handle_t m_ioHandle    { nullptr };
  esp_lcd_panel_handle_t    m_panelHandle { nullptr };

  DisplayTxDoneCallback m_txDoneCallback { nullptr };
  void*                 m_txDoneArg      { nullptr };
};

} // namespace esp32utilities::displays::ST77916
