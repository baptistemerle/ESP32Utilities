#pragma once

#include <driver/i2c_master.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"
#include "ESP32Utilities/Expanders/iexpander.h"

#include "configuration.h"

namespace esp32utilities::expanders::TCA9554
{

class Driver : public IExpander
{
public:
  explicit Driver(const Configuration& configuration);
  virtual ~Driver();

  esp_err_t init(i2c_master_bus_handle_t busHandle);
  esp_err_t setPinDirection(uint8_t pin, GPIODirection direction) override;
  esp_err_t setPinLevel(uint8_t pin, GPIOLevel level) override;
  esp_err_t readPinLevel(uint8_t pin, GPIOLevel* level) const override;

private:
  esp_err_t readRegister(uint8_t reg, uint8_t* value) const;
  esp_err_t writeRegister(uint8_t reg, uint8_t value) const;

private:
  const Configuration m_configuration;

  i2c_master_dev_handle_t m_deviceHandle { nullptr };

  uint8_t m_outputReg { 0xFF };
  uint8_t m_configReg { 0xFF };
};

} // namespace esp32utilities::expanders::TCA9554
