#ifndef QMI8658_DRIVER_H
#define QMI8658_DRIVER_H

#include "Interfaces/iaccelerometer.h"

#include <SensorQMI8658.hpp>

class QMI8658Driver_Configuration;

class QMI8658Driver : public IAccelerometer
{
public:
  QMI8658Driver(const QMI8658Driver_Configuration& configuration);

  bool init();

  virtual bool getValues(float& x, float& y, float& z) override;

private:
  const QMI8658Driver_Configuration& m_configuration;

  SensorQMI8658 m_qmiSensor;
};

#endif // QMI8658_DRIVER_H