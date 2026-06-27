#ifndef ST7701_DRIVER_H
#define ST7701_DRIVER_H

#include "Interfaces/gpio_types.h"
#include "Interfaces/idisplay_driver.h"

#include "driver/spi_master.h"

#include "esp_err.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"

#include <functional>

class ST7701Driver_Configuration;

using PinControlCallback = std::function<void(GPIO::Level level)>;

class ST7701Driver : public IDisplayDriver
{
public:
  ST7701Driver(const ST7701Driver_Configuration& configuration,
                     PinControlCallback          setCsCallback,
                     PinControlCallback          setResetCallback);
  virtual ~ST7701Driver();

  void init(DisplayTxDoneCallback callback, void* callbackArg) override;
  void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawBuffer) override;

  uint32_t width() const override;
  uint32_t height() const override;

  DisplayRenderMode preferredRenderMode() const override;
  void* getFrameBuffer(uint8_t index) const override;

private:
  void initST7701SRegisters();

  esp_err_t writeCommand9Bit(uint8_t cmd);
  esp_err_t writeData9Bit(uint8_t data);

private:
  const ST7701Driver_Configuration& m_configuration;

  PinControlCallback m_setCsCallback;
  PinControlCallback m_setResetCallback;

  esp_lcd_panel_handle_t m_panelHandle = nullptr;
  void*                  m_framebuffers[2] { nullptr, nullptr };

  DisplayTxDoneCallback m_txDoneCallback =    nullptr;
  void*                 m_txDoneCallbackArg = nullptr;

  spi_device_handle_t m_spiHandle = nullptr;
};

#endif // ST7701_DRIVER_H