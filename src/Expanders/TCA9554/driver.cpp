#include "ESP32Utilities/Expanders/TCA9554/driver.h"

#include <driver/i2c_master.h>
#include <esp_check.h>

#include "ESP32Utilities/Expanders/TCA9554/configuration.h"

namespace esp32utilities::expanders::TCA9554
{

namespace
{
constexpr uint8_t inputPortCommand =  0x00;
constexpr uint8_t outputPortCommand = 0x01;
constexpr uint8_t configPortCommand = 0x03;

constexpr char TAG[] = "TCA9554";
}

Driver::Driver(const Configuration& configuration)
  : m_configuration(configuration)
{
}

Driver::~Driver()
{
  if (m_deviceHandle)
  {
    i2c_master_bus_rm_device(m_deviceHandle);
    m_deviceHandle = nullptr;
  }
}

esp_err_t Driver::init(i2c_master_bus_handle_t busHandle)
{
  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address =  m_configuration.i2cAddress;
  dev_cfg.scl_speed_hz =    m_configuration.i2cSpeedHz;

  ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(busHandle, &dev_cfg, &m_deviceHandle), TAG, "Failed to add device");
  ESP_RETURN_ON_ERROR(readRegister(outputPortCommand, &m_outputReg), TAG, "Failed to read output port");
  ESP_RETURN_ON_ERROR(readRegister(configPortCommand, &m_configReg), TAG, "Failed to read configuration port");

  return ESP_OK;
}

esp_err_t Driver::setPinDirection(uint8_t pin, GPIODirection direction)
{
  if (pin > 7)
    return ESP_ERR_INVALID_ARG;

  if (direction == GPIODirection::Output)
  {
    m_configReg &= ~(1 << pin); // 0 = Output
  }
  else
  {
    m_configReg |= (1 << pin);  // 1 = Input
  }

  return writeRegister(configPortCommand, m_configReg);
}

esp_err_t Driver::setPinLevel(uint8_t pin, GPIOLevel level)
{
  if (pin > 7)
    return ESP_ERR_INVALID_ARG;

  if (level == GPIOLevel::High)
  {
    m_outputReg |= (1 << pin);
  }
  else
  {
    m_outputReg &= ~(1 << pin);
  }

  return writeRegister(outputPortCommand, m_outputReg);
}

esp_err_t Driver::readPinLevel(uint8_t pin, GPIOLevel* level) const
{
  if (pin > 7)
    return ESP_ERR_INVALID_ARG;

  uint8_t inputReg;
  esp_err_t err = readRegister(inputPortCommand, &inputReg);
  if (err != ESP_OK)
    return err;

  *level = ((inputReg & (1 << pin)) ? GPIOLevel::High : GPIOLevel::Low);

  return ESP_OK;
}

esp_err_t Driver::readRegister(uint8_t reg, uint8_t* value) const
{
  return i2c_master_transmit_receive(m_deviceHandle, &reg, 1, value, 1, -1);
}

esp_err_t Driver::writeRegister(uint8_t reg, uint8_t value) const
{
  uint8_t buffer[2] = { reg, value };

  return i2c_master_transmit(m_deviceHandle, buffer, 2, -1);
}

} // namespace esp32utilities::expanders::TCA9554
