#ifndef DISPLAY_ENGINE_H
#define DISPLAY_ENGINE_H

#include <lvgl.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class IDisplayDriver;

class DisplayEngine
{
public:
  DisplayEngine(IDisplayDriver& hardwareDriver);
  ~DisplayEngine();

  lv_display_t* init();

private:
  static void flushCallback(lv_display_t* display, const lv_area_t* area, uint8_t* colorData);
  static void onDriverTXDone(void* arg);

private:
  IDisplayDriver& m_hardwareDriver;

  void* m_buffer1 = nullptr;
  void* m_buffer2 = nullptr;

  lv_display_t*     m_lvglDisplay = nullptr;
  SemaphoreHandle_t m_dmaSemaphore = nullptr;
};

#endif // DISPLAY_ENGINE_H