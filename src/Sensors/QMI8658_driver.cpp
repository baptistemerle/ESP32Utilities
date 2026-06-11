#include "QMI8658_driver.h"

#include "QMI8658_driver_configuration.h"

QMI8658Driver::QMI8658Driver(const QMI8658Driver_Configuration& configuration)
  : m_configuration(configuration)
{
}

bool QMI8658Driver::init()
{
  if (!m_qmiSensor.begin(m_configuration.wire,
                         m_configuration.i2cAddress,
                         m_configuration.i2cPinSDA,
                         m_configuration.i2cPinSCL))
  {
    return false;
  }

  m_qmiSensor.configAccelerometer(m_configuration.range,
                                  m_configuration.odr,
                                  m_configuration.lpfMode);
  m_qmiSensor.enableAccelerometer();

  return true;
}

bool QMI8658Driver::getValues(float& x, float& y, float& z)
{
  return m_qmiSensor.getAccelerometer(x, y, z);
}