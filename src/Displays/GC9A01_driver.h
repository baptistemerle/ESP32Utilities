#ifndef GC9A01_DRIVER_H
#define GC9A01_DRIVER_H

#include "Interfaces/idisplay_driver.h"

#include "esp_lcd_panel_io.h"

class GC9A01Driver_Configuration;

class GC9A01Driver : public IDisplayDriver
{
public:
  GC9A01Driver(const GC9A01Driver_Configuration& configuration);
  ~GC9A01Driver() override;

  void init(DisplayTxDoneCallback callback, void* callbackArg) override;
  void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData) override;

  uint32_t width() const override;
  uint32_t height() const override;
  DisplayRenderMode preferredRenderMode() const override;
  bool requiresByteSwap() const override;

private:
  const GC9A01Driver_Configuration& m_configuration;

  esp_lcd_panel_io_handle_t m_ioHandle =    nullptr;
  esp_lcd_panel_handle_t    m_panelHandle = nullptr;

  DisplayTxDoneCallback m_txDoneCallback = nullptr;
  void*                 m_txDoneArg =      nullptr;
};

#endif // GC9A01_DRIVER_H