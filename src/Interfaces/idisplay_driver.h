#ifndef IDISPLAY_DRIVER_H
#define IDISPLAY_DRIVER_H

#include <stdint.h>

class IDisplayDriver
{
public:
  virtual void init() = 0;
  virtual void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void* rawData) = 0;

  virtual int width() const = 0;
  virtual int height() const = 0;
};

#endif // IDISPLAY_DRIVER_H