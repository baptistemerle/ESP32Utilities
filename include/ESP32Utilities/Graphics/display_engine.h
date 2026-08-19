#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <lvgl.h>

namespace esp32utilities::displays
{
  class IDisplayDriver;
}

namespace esp32utilities::graphics
{

class DisplayEngine
{
protected:
  using IDisplayDriver = esp32utilities::displays::IDisplayDriver;

public:
  DisplayEngine(IDisplayDriver& hardwareDriver);
  ~DisplayEngine();

  lv_display_t* init();

private:
  static void flushCallback(lv_display_t* display, const lv_area_t* area, uint8_t* colorData);
  static void onDriverTXDone(void* arg);

private:
  IDisplayDriver& m_hardwareDriver;

  void* m_buffer1 { nullptr };
  void* m_buffer2 { nullptr };

  lv_display_t*          m_lvglDisplay    { nullptr };
  volatile lv_display_t* m_pendingDisplay { nullptr };
};

} // namespace esp32utilities::graphics
