#ifndef IACCELEROMETER_H
#define IACCELEROMETER_H

class IAccelerometer
{
public:
  virtual bool getValues(float& x, float& y, float& z) = 0;
};

#endif // IACCELEROMETER_H