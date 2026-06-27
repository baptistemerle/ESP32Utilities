#ifndef TCA9554_DRIVER_H
#define TCA9554_DRIVER_H

#include "Interfaces/gpio_types.h"
#include "Interfaces/iexpander.h"

#include <driver/i2c_master.h>

class TCA9554Driver_Configuration;

class TCA9554Driver : public IExpander
{
public:
  explicit TCA9554Driver(const TCA9554Driver_Configuration& configuration);
  virtual ~TCA9554Driver();

  esp_err_t init(i2c_master_bus_handle_t busHandle);
  esp_err_t setPinDirection(uint8_t pin, GPIO::Direction direction) override;
  esp_err_t setPinLevel(uint8_t pin, GPIO::Level level) override;
  esp_err_t readPinLevel(uint8_t pin, GPIO::Level* level) const override;

private:
  esp_err_t readRegister(uint8_t reg, uint8_t* value) const;
  esp_err_t writeRegister(uint8_t reg, uint8_t value) const;

private:
  const TCA9554Driver_Configuration& m_configuration;

  i2c_master_dev_handle_t m_deviceHandle = nullptr;

  uint8_t m_outputReg = 0xFF;
  uint8_t m_configReg = 0xFF;
};

#endif // TCA9554_DRIVER_H