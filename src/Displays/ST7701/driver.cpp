#include "ESP32Utilities/Displays/ST7701/driver.h"

#include <cstring>

#include "calibration.h"

namespace esp32utilities::displays::ST7701
{

Driver::Driver(const Configuration&     configuration,
                     PinControlCallback setCsCallback,
                     PinControlCallback setResetCallback)
  : m_configuration(configuration)
  , m_setCsCallback(std::move(setCsCallback))
  , m_setResetCallback(std::move(setResetCallback))
{
}

Driver::~Driver()
{
  if (m_panelHandle)
  {
    esp_lcd_panel_del(m_panelHandle);
    m_panelHandle = nullptr;
  }

  if (m_spiHandle)
  {
    spi_bus_remove_device(m_spiHandle);
    m_spiHandle = nullptr;
  }
}

void Driver::init(DisplayTxDoneCallback callback, void* callbackArg)
{
  initST7701SRegisters();

  esp_lcd_rgb_panel_config_t panel_config = {};
  panel_config.clk_src =        LCD_CLK_SRC_DEFAULT;
  panel_config.timings =        m_configuration.timings;
  panel_config.data_width =     16;
  panel_config.de_gpio_num =    m_configuration.pinDE;
  panel_config.hsync_gpio_num = m_configuration.pinHSYNC;
  panel_config.vsync_gpio_num = m_configuration.pinVSYNC;
  panel_config.pclk_gpio_num =  m_configuration.pinPCLK;

  for (int i = 0; i < 16; i++)
  {
    panel_config.data_gpio_nums[i] = m_configuration.dataPins[i];
  }

  panel_config.num_fbs =               2;
  panel_config.flags.fb_in_psram =     true;
  panel_config.bounce_buffer_size_px = m_configuration.screenWidth * 60;

  ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(m_panelHandle));

  ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(m_panelHandle, 2, &m_framebuffers[0], &m_framebuffers[1]));

  std::memset(m_framebuffers[0], 0x00, m_configuration.screenWidth * m_configuration.screenHeight * sizeof(uint16_t));
  std::memset(m_framebuffers[1], 0x00, m_configuration.screenWidth * m_configuration.screenHeight * sizeof(uint16_t));

  esp_lcd_rgb_panel_event_callbacks_t cbs = {};
  cbs.on_vsync = [](esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx) -> bool
  {
    auto* driver = static_cast<Driver*>(user_ctx);
    BaseType_t high_task_awoken = pdFALSE;

    if (driver->m_flushTaskHandle != nullptr)
    {
      vTaskNotifyGiveFromISR(driver->m_flushTaskHandle, &high_task_awoken);
    }
    return high_task_awoken == pdTRUE;
  };

  ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(m_panelHandle, &cbs, this));
}

void Driver::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawBuffer)
{
  m_flushTaskHandle = xTaskGetCurrentTaskHandle();
  ulTaskNotifyValueClear(NULL, ULONG_MAX);

  esp_lcd_panel_draw_bitmap(m_panelHandle, 0, 0, m_configuration.screenWidth, m_configuration.screenHeight, rawBuffer);

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  m_flushTaskHandle = nullptr;
}

uint32_t Driver::width() const
{
  return m_configuration.screenWidth;
}

uint32_t Driver::height() const
{
  return m_configuration.screenHeight;
}

DisplayRenderMode Driver::preferredRenderMode() const
{
  return DisplayRenderMode::Direct;
}

void* Driver::getFrameBuffer(uint8_t index) const
{
  if (index < 2)
  {
    return m_framebuffers[index];
  }

  return nullptr;
}

void Driver::initST7701SRegisters()
{
  spi_device_interface_config_t dev_cfg = {};
  dev_cfg.clock_speed_hz = m_configuration.spiSpeedHz;
  dev_cfg.mode =           0;
  dev_cfg.spics_io_num =   -1;
  dev_cfg.queue_size =     1;
  dev_cfg.command_bits =   1;
  dev_cfg.address_bits =   8;
  dev_cfg.dummy_bits =     0;

  ESP_ERROR_CHECK(spi_bus_add_device(m_configuration.spiHost, &dev_cfg, &m_spiHandle));

  reset();

  m_setCsCallback(GPIOLevel::Low);
  vTaskDelay(pdMS_TO_TICKS(10));

  st7701_vendor_config_t vendor_config =
  {
    .init_cmds = factoryCommands,
    .init_cmds_size = sizeof(factoryCommands) / sizeof(st7701_lcd_init_cmd_t),
  };

  for (size_t i = 0; i < vendor_config.init_cmds_size; i++)
  {
    const auto& cmdEntry = vendor_config.init_cmds[i];

    writeCommand9Bit(cmdEntry.cmd);

    for (uint8_t j = 0; j < cmdEntry.data_bytes; j++)
    {
      writeData9Bit(cmdEntry.data[j]);
    }

    if (cmdEntry.delay_ms > 0)
    {
      vTaskDelay(pdMS_TO_TICKS(cmdEntry.delay_ms));
    }
  }

  m_setCsCallback(GPIOLevel::High);
}

void Driver::reset()
{
  m_setResetCallback(GPIOLevel::Low);
  vTaskDelay(pdMS_TO_TICKS(50));
  m_setResetCallback(GPIOLevel::High);
  vTaskDelay(pdMS_TO_TICKS(120));
}

esp_err_t Driver::writeCommand9Bit(uint8_t cmd)
{
  spi_transaction_t t = {};
  t.flags =  0;
  t.length = 0;
  t.cmd =    0;
  t.addr =   cmd;

  return spi_device_polling_transmit(m_spiHandle, &t);
}

esp_err_t Driver::writeData9Bit(uint8_t data)
{
  spi_transaction_t t = {};
  t.flags =  0;
  t.length = 0;
  t.cmd =    1;
  t.addr =   data;

  return spi_device_polling_transmit(m_spiHandle, &t);
}

} // namespace esp32utilities::displays::ST7701
