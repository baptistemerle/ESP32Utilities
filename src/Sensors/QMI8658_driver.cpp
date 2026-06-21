#include "QMI8658_driver.h"

#include "QMI8658_driver_configuration.h"

#include <esp_check.h>

namespace
{
constexpr uint8_t regWhoAmI = 0x00;
constexpr uint8_t regCtrl1 =  0x02;
constexpr uint8_t regCtrl2 =  0x03;
constexpr uint8_t regCtrl5 =  0x06;
constexpr uint8_t regCtrl7 =  0x08;
constexpr uint8_t regAxL =    0x35; // Start of accelerometer data

constexpr uint8_t regWhoAmiExpectedValue = 0x05;

constexpr uint8_t ctrl1Config = 0x60; // Auto address increment and serial configuration
constexpr uint8_t ctrl7Config = 0x01; // Accelerometer only

constexpr char TAG[] = "QMI8658";
}

QMI8658Driver::QMI8658Driver(const QMI8658Driver_Configuration& configuration)
  : m_configuration(configuration)
{
}

esp_err_t QMI8658Driver::init(i2c_master_bus_handle_t bus_handle)
{
  i2c_device_config_t dev_config = {};
  dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_config.device_address = m_configuration.i2cAddress;
  dev_config.scl_speed_hz = m_configuration.i2cSpeedHz;

  ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus_handle, &dev_config, &m_deviceHandle), TAG, "Failed to add device");

  uint8_t whoami = 0;
  ESP_RETURN_ON_ERROR(readRegisters(regWhoAmI, &whoami, 1), TAG, "Failed to read whoami");

  if (whoami != regWhoAmiExpectedValue)
    return ESP_FAIL;

  ESP_RETURN_ON_ERROR(writeRegister(regCtrl1, ctrl1Config), TAG, "Failed to write CTRL1");

  uint8_t ctrl2_value = (static_cast<uint8_t>(m_configuration.range) << 4) |
                        (static_cast<uint8_t>(m_configuration.odr));
  ESP_RETURN_ON_ERROR(writeRegister(regCtrl2, ctrl2_value), TAG, "Failed to write CTRL2");

  uint8_t ctrl5_value = static_cast<uint8_t>(m_configuration.lpfMode);
  ESP_RETURN_ON_ERROR(writeRegister(regCtrl5, ctrl5_value), TAG, "Failed to write CTRL5");

  ESP_RETURN_ON_ERROR(writeRegister(regCtrl7, ctrl7Config), TAG, "Failed to write CTRL7");

  return ESP_OK;
}

esp_err_t QMI8658Driver::getValues(float& x, float& y, float& z) const
{
  uint8_t buffer[6];

  ESP_RETURN_ON_ERROR(readRegisters(regAxL, buffer, 6), TAG, "Failed to read accelerometer data");

  int16_t rawX = static_cast<int16_t>((buffer[1] << 8) | buffer[0]);
  int16_t rawY = static_cast<int16_t>((buffer[3] << 8) | buffer[2]);
  int16_t rawZ = static_cast<int16_t>((buffer[5] << 8) | buffer[4]);

  float scaleFactor = 1.0f;
  switch (m_configuration.range)
  {
    case QMI8658Driver_AccelRange::Range_2G:
      scaleFactor = 16384.0f;
      break;
    case QMI8658Driver_AccelRange::Range_4G:
      scaleFactor = 8192.0f;
      break;
    case QMI8658Driver_AccelRange::Range_8G:
      scaleFactor = 4096.0f;
      break;
    case QMI8658Driver_AccelRange::Range_16G:
      scaleFactor = 2048.0f;
      break;
  }

  x = static_cast<float>(rawX) / scaleFactor;
  y = static_cast<float>(rawY) / scaleFactor;
  z = static_cast<float>(rawZ) / scaleFactor;

  return ESP_OK;
}

esp_err_t QMI8658Driver::writeRegister(uint8_t reg, uint8_t value) const
{
  uint8_t write_buf[2] = { reg, value };

  return i2c_master_transmit(m_deviceHandle, write_buf, 2, -1);
}

esp_err_t QMI8658Driver::readRegisters(uint8_t reg, uint8_t* data, size_t len) const
{
  return i2c_master_transmit_receive(m_deviceHandle, &reg, 1, data, len, -1);
}