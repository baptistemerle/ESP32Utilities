#pragma once

#include <driver/i2c_master.h>

#include "ESP32Utilities/Sensors/iaccelerometer.h"

#include "configuration.h"

namespace esp32utilities::sensors::QMI8658
{

class Driver : public IAccelerometer
{
public:
  explicit Driver(const Configuration& configuration);

  esp_err_t init(i2c_master_bus_handle_t bus_handle);

  esp_err_t getValues(float& x, float& y, float& z) const override;

private:
  esp_err_t writeRegister(uint8_t reg, uint8_t value) const;
  esp_err_t readRegisters(uint8_t reg, uint8_t* data, size_t length) const;

private:
  const Configuration m_configuration;

  i2c_master_dev_handle_t m_deviceHandle;
};

} // namespace esp32utilities::sensors::QMI8658
