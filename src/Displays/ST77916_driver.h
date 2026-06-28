#ifndef ST77916_DRIVER_H
#define ST77916_DRIVER_H

#include "Interfaces/gpio_types.h"
#include "Interfaces/idisplay_driver.h"

#include <esp_lcd_panel_io.h>

#include <functional>

class ST77916Driver_Configuration;

using PinControlCallback = std::function<void(GPIO::Level level)>;

class ST77916Driver : public IDisplayDriver
{
public:
  ST77916Driver(const ST77916Driver_Configuration& configuration,
                      PinControlCallback           setResetCallback);
  virtual ~ST77916Driver();

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
  const ST77916Driver_Configuration& m_configuration;

  PinControlCallback m_setResetCallback;

  esp_lcd_panel_io_handle_t m_ioHandle =    nullptr;
  esp_lcd_panel_handle_t    m_panelHandle = nullptr;

  DisplayTxDoneCallback m_txDoneCallback = nullptr;
  void*                 m_txDoneArg =      nullptr;
};

#endif // ST77916_DRIVER_H