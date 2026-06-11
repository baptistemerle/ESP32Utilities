#ifndef DISPLAY_ENGINE_H
#define DISPLAY_ENGINE_H

#include <lvgl.h>

class IDisplayDriver;

class DisplayEngine
{
public:
  DisplayEngine(IDisplayDriver& hardwareDriver, uint16_t bufferHeightRatio = 10);
  ~DisplayEngine();

  lv_disp_t* init();

private:
  static void flush(lv_disp_drv_t* display, const lv_area_t* area, lv_color_t* colorData);

private:
  IDisplayDriver&    m_hardwareDriver;
  uint32_t           m_pixelCount;
  lv_disp_draw_buf_t m_drawBuffer;
  lv_color_t*        m_buffer = nullptr;
  lv_disp_drv_t      m_lvglDriver;
  lv_disp_t*         m_lvglDisplay = nullptr;
};

#endif // DISPLAY_ENGINE_H