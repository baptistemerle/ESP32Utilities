#include "display_engine.h"

#include "Interfaces/idisplay_driver.h"

DisplayEngine::DisplayEngine(IDisplayDriver& hardwareDriver, uint16_t bufferHeightRatio)
  : m_hardwareDriver(hardwareDriver)
  , m_pixelCount(hardwareDriver.width() * (hardwareDriver.height() / bufferHeightRatio))
{
}

DisplayEngine::~DisplayEngine()
{
  if (m_lvglDisplay)
  {
    lv_disp_remove(m_lvglDisplay);
  }

  if (m_buffer)
  {
    delete[] m_buffer;
    m_buffer = nullptr;
  }
}

lv_disp_t* DisplayEngine::init()
{
  m_hardwareDriver.init();

  m_buffer = new lv_color_t[m_pixelCount];
  lv_disp_draw_buf_init(&m_drawBuffer, m_buffer, nullptr, m_pixelCount);

  lv_disp_drv_init(&m_lvglDriver);

  m_lvglDriver.hor_res =    m_hardwareDriver.width();
  m_lvglDriver.ver_res =    m_hardwareDriver.height();
  m_lvglDriver.flush_cb =  &DisplayEngine::flush;
  m_lvglDriver.draw_buf =  &m_drawBuffer;
  m_lvglDriver.user_data =  this;

  m_lvglDisplay = lv_disp_drv_register(&m_lvglDriver);
  return m_lvglDisplay;
}

void DisplayEngine::flush(lv_disp_drv_t* display, const lv_area_t* area, lv_color_t* colorData)
{
  DisplayEngine* instance = static_cast<DisplayEngine*>(display->user_data);

  instance->m_hardwareDriver.flush(area->x1, area->y1, area->x2, area->y2, colorData);

  lv_disp_flush_ready(display);
}