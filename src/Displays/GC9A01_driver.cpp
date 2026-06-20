#include "GC9A01_driver.h"

#include "GC9A01_driver_calibration.h"
#include "GC9A01_driver_configuration.h"

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_gc9a01.h"
#include "esp_lcd_panel_ops.h"

GC9A01Driver::GC9A01Driver(const GC9A01Driver_Configuration& configuration)
  : m_configuration(configuration)
{
}

GC9A01Driver::~GC9A01Driver()
{
  if (m_panelHandle)
  {
    esp_lcd_panel_del(m_panelHandle);
  }

  if (m_ioHandle)
  {
    esp_lcd_panel_io_del(m_ioHandle);
  }

  spi_bus_free(m_configuration.spiHost);
}

void GC9A01Driver::init(DisplayTxDoneCallback callback, void* callbackArg)
{
  m_txDoneCallback = callback;
  m_txDoneArg = callbackArg;

  spi_bus_config_t busConfig = {};
  busConfig.sclk_io_num = m_configuration.spiPinClock;
  busConfig.mosi_io_num = m_configuration.spiPinMOSI;
  busConfig.miso_io_num = -1;
  busConfig.quadwp_io_num = -1;
  busConfig.quadhd_io_num = -1;
  busConfig.max_transfer_sz = m_configuration.screenWidth * 40 * sizeof(uint16_t);

  ESP_ERROR_CHECK(spi_bus_initialize(m_configuration.spiHost, &busConfig, SPI_DMA_CH_AUTO));

  esp_lcd_panel_io_spi_config_t ioConfig = {};
  ioConfig.dc_gpio_num = m_configuration.pinDataCommand;
  ioConfig.cs_gpio_num = m_configuration.spiPinChipSelect;
  ioConfig.pclk_hz = m_configuration.spiFrequencyWrite;
  ioConfig.lcd_cmd_bits = 8;
  ioConfig.lcd_param_bits = 8;
  ioConfig.spi_mode = 0;
  ioConfig.trans_queue_depth = 10;

  /**
   * @brief This lambda function acts as the SPI/DMA transfer completion interrupt handler.
   * It executes directly within the Hardware ISR (Interrupt Service Routine) context.
   * DO NOT invoke any blocking FreeRTOS functions, allocations, or direct LVGL core logic here.
   */
  ioConfig.on_color_trans_done = [](esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t* eventData, void* userContext) -> bool
  {
    auto* self = static_cast<GC9A01Driver*>(userContext);
    if (self->m_txDoneCallback)
    {
      self->m_txDoneCallback(self->m_txDoneArg);
    }
    return false;
  };
  ioConfig.user_ctx = this;

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)m_configuration.spiHost, &ioConfig, &m_ioHandle));

  gc9a01_vendor_config_t vendor_config =
  {
    .init_cmds = gc9a01_factoryCommands,
    .init_cmds_size = sizeof(gc9a01_factoryCommands) / sizeof(gc9a01_lcd_init_cmd_t),
  };

  esp_lcd_panel_dev_config_t panel_config = {};
  panel_config.reset_gpio_num = m_configuration.pinReset;
  panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
  panel_config.bits_per_pixel = 16;
  panel_config.vendor_config = &vendor_config;

  ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(m_ioHandle, &panel_config, &m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(m_panelHandle, false, false));
  ESP_ERROR_CHECK(esp_lcd_panel_invert_color(m_panelHandle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(m_panelHandle, true));
}

void GC9A01Driver::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData)
{
  esp_lcd_panel_draw_bitmap(m_panelHandle, x1, y1, x2 + 1, y2 + 1, rawData);
}

int GC9A01Driver::width() const
{
  return m_configuration.screenWidth;
}

int GC9A01Driver::height() const
{
  return m_configuration.screenHeight;
}

DisplayRenderMode GC9A01Driver::preferredRenderMode() const
{
  return DisplayRenderMode::Partial;
}

bool GC9A01Driver::requiresByteSwap() const
{
  return true;
}