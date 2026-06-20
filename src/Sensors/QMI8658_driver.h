#ifndef QMI8658_DRIVER_H
#define QMI8658_DRIVER_H

#include "Interfaces/iaccelerometer.h"

#include <driver/i2c_master.h>

class QMI8658Driver_Configuration;

class QMI8658Driver : public IAccelerometer
{
public:
  QMI8658Driver(const QMI8658Driver_Configuration& configuration);

  bool init(i2c_master_bus_handle_t bus_handle);

  bool getValues(float& x, float& y, float& z) override;

private:
  bool writeRegister(uint8_t reg, uint8_t value);
  bool readRegisters(uint8_t reg, uint8_t* data, size_t length);

private:
  const QMI8658Driver_Configuration& m_configuration;

  i2c_master_dev_handle_t m_deviceHandle;
};

#endif // QMI8658_DRIVER_H