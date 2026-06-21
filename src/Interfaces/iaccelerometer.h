#ifndef IACCELEROMETER_H
#define IACCELEROMETER_H

#include <esp_err.h>

class IAccelerometer
{
public:
  virtual ~IAccelerometer() = default;

  virtual esp_err_t getValues(float& x, float& y, float& z) const = 0;
};

#endif // IACCELEROMETER_H