#include "ST77916_driver.h"

#include "ST77916_driver_calibration.h"
#include "ST77916_driver_configuration.h"

#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_lcd_st77916.h>
#include <esp_lcd_panel_ops.h>
#include <freertos/FreeRTOS.h>

ST77916Driver::ST77916Driver(const ST77916Driver_Configuration& configuration,
                                   PinControlCallback           setResetCallback)
  : m_configuration(configuration)
  , m_setResetCallback(std::move(setResetCallback))
{
}

ST77916Driver::~ST77916Driver()
{
  if (m_panelHandle)
  {
    esp_lcd_panel_del(m_panelHandle);
    m_panelHandle = nullptr;
  }

  if (m_ioHandle)
  {
    esp_lcd_panel_io_del(m_ioHandle);
    m_ioHandle = nullptr;
  }
}

void ST77916Driver::init(DisplayTxDoneCallback callback, void* callbackArg)
{
  m_txDoneCallback = callback;
  m_txDoneArg =      callbackArg;

  reset();

  ScreenRevision revision = probeScreenRevision();

  esp_lcd_panel_io_spi_config_t ioConfig = {};
  ioConfig.dc_gpio_num =       GPIO_NUM_NC;
  ioConfig.cs_gpio_num =       m_configuration.spiPinChipSelect;
  ioConfig.pclk_hz =           m_configuration.spiSpeedHz;
  ioConfig.lcd_cmd_bits =      32;
  ioConfig.lcd_param_bits =    8;
  ioConfig.spi_mode =          0;
  ioConfig.trans_queue_depth = 10;
  ioConfig.flags.quad_mode =   true;

  /**
   * @brief This lambda function acts as the SPI/DMA transfer completion interrupt handler.
   * It executes directly within the Hardware ISR (Interrupt Service Routine) context.
   * DO NOT invoke any blocking FreeRTOS functions, allocations, or direct LVGL core logic here.
   */
  ioConfig.on_color_trans_done = [](esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t* eventData, void* userContext) -> bool
  {
    auto* self = static_cast<ST77916Driver*>(userContext);
    if (self->m_txDoneCallback)
    {
      self->m_txDoneCallback(self->m_txDoneArg);
    }
    return false;
  };
  ioConfig.user_ctx = this;

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(static_cast<esp_lcd_spi_bus_handle_t>(m_configuration.spiHost), &ioConfig, &m_ioHandle));

  st77916_vendor_config_t vendor_config = {};
  vendor_config.flags.use_qspi_interface = 1;
  if (revision == ScreenRevision::Case2)
  {
    vendor_config.init_cmds =      st77916_factoryCommands;
    vendor_config.init_cmds_size = sizeof(st77916_factoryCommands) / sizeof(st77916_lcd_init_cmd_t);
  }
  else
  {
    vendor_config.init_cmds =      nullptr;
    vendor_config.init_cmds_size = 0;
  }

  esp_lcd_panel_dev_config_t panel_config = {};
  panel_config.reset_gpio_num = m_configuration.pinReset;
  panel_config.rgb_ele_order =  LCD_RGB_ELEMENT_ORDER_RGB;
  panel_config.bits_per_pixel = 16;
  panel_config.vendor_config =  &vendor_config;

  reset();

  ESP_ERROR_CHECK(esp_lcd_new_panel_st77916(m_ioHandle, &panel_config, &m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(m_panelHandle));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(m_panelHandle, false, false));
  ESP_ERROR_CHECK(esp_lcd_panel_invert_color(m_panelHandle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(m_panelHandle, true));
}

void ST77916Driver::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData)
{
  esp_lcd_panel_draw_bitmap(m_panelHandle, x1, y1, x2 + 1, y2 + 1, rawData);
}

uint32_t ST77916Driver::width() const
{
  return m_configuration.screenWidth;
}

uint32_t ST77916Driver::height() const
{
  return m_configuration.screenHeight;
}

DisplayRenderMode ST77916Driver::preferredRenderMode() const
{
  return DisplayRenderMode::Partial;
}

bool ST77916Driver::requiresByteSwap() const
{
  return true;
}

void ST77916Driver::reset()
{
  m_setResetCallback(GPIO::Level::Low);
  vTaskDelay(pdMS_TO_TICKS(50));
  m_setResetCallback(GPIO::Level::High);
  vTaskDelay(pdMS_TO_TICKS(120));
}

// Extracted and adapted from Waveshare sample code
ST77916Driver::ScreenRevision ST77916Driver::probeScreenRevision() const
{
  esp_lcd_panel_io_spi_config_t probeConfig = {};
  probeConfig.dc_gpio_num =       GPIO_NUM_NC;
  probeConfig.cs_gpio_num =       m_configuration.spiPinChipSelect;
  probeConfig.pclk_hz =           3 * 1000 * 1000;
  probeConfig.lcd_cmd_bits =      32;
  probeConfig.lcd_param_bits =    8;
  probeConfig.spi_mode =          0;
  probeConfig.trans_queue_depth = 2;
  probeConfig.flags.quad_mode =   true;

  esp_lcd_panel_io_handle_t temporaryIoHandle = nullptr;
  ScreenRevision detectedRevision = ScreenRevision::Case1;

  esp_err_t ret = esp_lcd_new_panel_io_spi(
    static_cast<esp_lcd_spi_bus_handle_t>(m_configuration.spiHost),
    &probeConfig,
    &temporaryIoHandle
  );

  if (ret == ESP_OK)
  {
    int lcdCmd = (0x0B << 24) | (0x04 << 8);
    uint8_t register_data[4] = {0};

    if (esp_lcd_panel_io_rx_param(temporaryIoHandle, lcdCmd, register_data, sizeof(register_data)) == ESP_OK)
    {
      // Case 2 signature : [0x00, 0x02, 0x7F, 0x7F]
      if (register_data[0] == 0x00 && register_data[1] == 0x02 && register_data[2] == 0x7F && register_data[3] == 0x7F)
      {
        detectedRevision = ScreenRevision::Case2;
      }
    }

    esp_lcd_panel_io_del(temporaryIoHandle);
  }

  return detectedRevision;
}