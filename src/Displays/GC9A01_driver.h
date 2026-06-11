#ifndef GC9A01_DRIVER_H
#define GC9A01_DRIVER_H

#include "Interfaces/idisplay_driver.h"

#include <LovyanGFX.hpp>

class GC9A01Driver_Configuration;

class GC9A01Driver : public IDisplayDriver
{
public:
  GC9A01Driver(const GC9A01Driver_Configuration& configuration);

  virtual void init() override;
  virtual void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void* rawData) override;

  virtual int width() const override;
  virtual int height() const override;

private:
  const GC9A01Driver_Configuration& m_configuration;

  lgfx::LGFX_Device  m_device;
  lgfx::Panel_GC9A01 m_panelInstance;
  lgfx::Bus_SPI      m_busInstance;
};

#endif // GC9A01_DRIVER_H