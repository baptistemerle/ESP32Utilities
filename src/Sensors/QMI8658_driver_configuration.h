#ifndef QMI8658_DRIVER_CONFIGURATION_H
#define QMI8658_DRIVER_CONFIGURATION_H

#include <SensorQMI8658.hpp>

struct QMI8658Driver_Configuration
{
  TwoWire& wire;
  uint8_t  i2cAddress;
  int      i2cPinSDA;
  int      i2cPinSCL;

  SensorQMI8658::AccelRange range;
  SensorQMI8658::AccelODR   odr;
  SensorQMI8658::LpfMode    lpfMode;
};

#endif // QMI8658_DRIVER_CONFIGURATION_H