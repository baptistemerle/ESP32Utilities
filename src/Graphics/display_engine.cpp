#include "display_engine.h"

#include "Interfaces/idisplay_driver.h"

#include "esp_heap_caps.h"

DisplayEngine::DisplayEngine(IDisplayDriver& hardwareDriver)
  : m_hardwareDriver(hardwareDriver)
{
}

DisplayEngine::~DisplayEngine()
{
  if (m_lvglDisplay)
  {
    lv_display_delete(m_lvglDisplay);
  }

  if (m_buffer1)
  {
    heap_caps_free(m_buffer1);
  }

  if (m_buffer2)
  {
    heap_caps_free(m_buffer2);
  }
}

lv_display_t* DisplayEngine::init()
{
  m_dmaSemaphore = xSemaphoreCreateBinary();
  if (!m_dmaSemaphore)
    return nullptr;

  xSemaphoreGive(m_dmaSemaphore);

  m_hardwareDriver.init(&DisplayEngine::onDriverTXDone, this);

  m_lvglDisplay = lv_display_create(m_hardwareDriver.width(), m_hardwareDriver.height());
  if (!m_lvglDisplay)
    return nullptr;

  if (m_hardwareDriver.requiresByteSwap())
  {
    lv_display_set_color_format(m_lvglDisplay, LV_COLOR_FORMAT_RGB565_SWAPPED);
  }
  else
  {
    lv_display_set_color_format(m_lvglDisplay, LV_COLOR_FORMAT_RGB565);
  }

  lv_display_set_user_data(m_lvglDisplay, this);
  lv_display_set_flush_cb(m_lvglDisplay, &DisplayEngine::flushCallback);

  if (m_hardwareDriver.preferredRenderMode() == DisplayRenderMode::Partial)
  {
    uint32_t bufferSize = m_hardwareDriver.width() * (m_hardwareDriver.height() / 10) * sizeof(uint16_t);

    m_buffer1 = heap_caps_malloc(bufferSize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    m_buffer2 = heap_caps_malloc(bufferSize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lv_display_set_buffers(m_lvglDisplay, m_buffer1, m_buffer2, bufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);
  }
  else
  {
    void* fb1 = m_hardwareDriver.getFramebuffer(0);
    void* fb2 = m_hardwareDriver.getFramebuffer(1);
    uint32_t fbSize = m_hardwareDriver.width() * m_hardwareDriver.height() * sizeof(uint16_t);

    lv_display_set_buffers(m_lvglDisplay, fb1, fb2, fbSize, LV_DISPLAY_RENDER_MODE_DIRECT);
  }

  return m_lvglDisplay;
}

void DisplayEngine::flushCallback(lv_display_t* display, const lv_area_t* area, uint8_t* colorData)
{
  DisplayEngine* instance = static_cast<DisplayEngine*>(lv_display_get_user_data(display));

  xSemaphoreTake(instance->m_dmaSemaphore, portMAX_DELAY);

  instance->m_hardwareDriver.flush(area->x1, area->y1, area->x2, area->y2, colorData);

  lv_display_flush_ready(display);
}

/**
 * @brief Callback triggered when the hardware driver finishes sending pixels via DMA.
 * @note This function executes inside the Hardware ISR context.
 * It must remain ultra-fast and only set an atomic/volatile flag to defer processing to the main task.
 */
void DisplayEngine::onDriverTXDone(void* arg)
{
  auto* instance = static_cast<DisplayEngine*>(arg);
  if (instance && instance->m_dmaSemaphore)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(instance->m_dmaSemaphore, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
      portYIELD_FROM_ISR();
    }
  }
}
