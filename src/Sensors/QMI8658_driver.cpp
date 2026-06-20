#include "QMI8658_driver.h"

#include "QMI8658_driver_configuration.h"

namespace
{
static const uint8_t regWhoAmI = 0x00;
static const uint8_t regCtrl1 =  0x02;
static const uint8_t regCtrl2 =  0x03;
static const uint8_t regCtrl5 =  0x06;
static const uint8_t regCtrl7 =  0x08;
static const uint8_t regAxL =    0x35; // Start of accelerometer data

static const uint8_t regWhoAmiExpectedValue = 0x05;

static const uint8_t ctrl1Config = 0x60; // Auto address increment and serial configuration
static const uint8_t ctrl7Config = 0x01; // Accelerometer only
}

QMI8658Driver::QMI8658Driver(const QMI8658Driver_Configuration& configuration)
  : m_configuration(configuration)
{
}

bool QMI8658Driver::init(i2c_master_bus_handle_t bus_handle)
{
  i2c_device_config_t dev_config = {};
  dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_config.device_address = m_configuration.i2cAddress;
  dev_config.scl_speed_hz = m_configuration.i2cSpeedHz;

  esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_config, &m_deviceHandle);
  if (err != ESP_OK)
    return false;

  uint8_t whoami = 0;
  if (!readRegisters(regWhoAmI, &whoami, 1) || whoami != regWhoAmiExpectedValue)
    return false;

  if (!writeRegister(regCtrl1, ctrl1Config))
    return false;

  uint8_t ctrl2_value = (static_cast<uint8_t>(m_configuration.range) << 4) |
                        (static_cast<uint8_t>(m_configuration.odr));
  if (!writeRegister(regCtrl2, ctrl2_value))
    return false;

  uint8_t ctrl5_value = static_cast<uint8_t>(m_configuration.lpfMode);
  if (!writeRegister(regCtrl5, ctrl5_value))
    return false;

  if (!writeRegister(regCtrl7, ctrl7Config))
    return false;

  return true;
}

bool QMI8658Driver::getValues(float& x, float& y, float& z)
{
  uint8_t buffer[6];

  if (!readRegisters(regAxL, buffer, 6))
    return false;

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

  return true;
}

bool QMI8658Driver::writeRegister(uint8_t reg, uint8_t value)
{
  uint8_t write_buf[2] = { reg, value };

  return i2c_master_transmit(m_deviceHandle, write_buf, 2, -1) == ESP_OK;
}

bool QMI8658Driver::readRegisters(uint8_t reg, uint8_t* data, size_t len)
{
  return i2c_master_transmit_receive(m_deviceHandle, &reg, 1, data, len, -1) == ESP_OK;
}