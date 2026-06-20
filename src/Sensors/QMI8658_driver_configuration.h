#ifndef QMI8658_DRIVER_CONFIGURATION_H
#define QMI8658_DRIVER_CONFIGURATION_H

#include <stdint.h>

enum class QMI8658Driver_AccelRange
{
  Range_2G  = 0x00,
  Range_4G  = 0x01,
  Range_8G  = 0x02,
  Range_16G = 0x03,
};

enum class QMI8658Driver_AccelODR
{
  ODR_1000Hz = 0x03,
  ODR_500Hz  = 0x04,
  ODR_250Hz  = 0x05,
  ODR_125Hz  = 0x06,
  ODR_62_5Hz = 0x07,
};

enum class QMI8658Driver_LPFMode
{
  Disabled   = 0x00,
  Mode_2_66  = 0x01,
  Mode_3_63  = 0x03,
  Mode_5_39  = 0x05,
  Mode_13_37 = 0x07,
};

struct QMI8658Driver_Configuration
{
  uint8_t  i2cAddress;
  uint32_t i2cSpeedHz;

  QMI8658Driver_AccelRange range;
  QMI8658Driver_AccelODR   odr;
  QMI8658Driver_LPFMode    lpfMode;
};

#endif // QMI8658_DRIVER_CONFIGURATION_H