#include "GC9A01_driver.h"

#include "GC9A01_driver_configuration.h"

GC9A01Driver::GC9A01Driver(const GC9A01Driver_Configuration& configuration)
  : m_configuration(configuration)
{
}

void GC9A01Driver::init()
{
  {
    auto cfg = m_busInstance.config();

    cfg.spi_host =   m_configuration.spiHost;
    cfg.spi_mode =   0;
    cfg.freq_write = m_configuration.spiFrequencyWrite;
    cfg.pin_sclk =   m_configuration.spiPinClock;
    cfg.pin_mosi =   m_configuration.spiPinMOSI;
    cfg.pin_miso =   -1;
    cfg.pin_dc =     m_configuration.pinDataCommand;

    m_busInstance.config(cfg);
    m_panelInstance.setBus(&m_busInstance);
  }

  {
    auto cfg = m_panelInstance.config();

    cfg.pin_cs           = m_configuration.spiPinChipSelect;
    cfg.pin_rst          = m_configuration.pinReset;
    cfg.pin_busy         = -1;
    cfg.panel_width      = m_configuration.screenWidth;
    cfg.panel_height     = m_configuration.screenHeight;
    cfg.offset_x         = 0;
    cfg.offset_y         = 0;
    cfg.invert           = true;

    m_panelInstance.config(cfg);
  }

  m_device.setPanel(&m_panelInstance);
  m_device.init();
}

void GC9A01Driver::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void* rawData)
{
  uint32_t w = (x2 - x1 + 1);
  uint32_t h = (y2 - y1 + 1);

  m_device.startWrite();
  m_device.setAddrWindow(x1, y1, w, h);
  m_device.writePixels((lgfx::rgb565_t *)rawData, w * h);
  m_device.endWrite();
}

int GC9A01Driver::width() const
{
  return m_configuration.screenWidth;
}

int GC9A01Driver::height() const
{
  return m_configuration.screenHeight;
}