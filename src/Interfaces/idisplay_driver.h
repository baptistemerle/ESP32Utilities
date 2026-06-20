#ifndef IDISPLAY_DRIVER_H
#define IDISPLAY_DRIVER_H

#include <stdint.h>

enum class DisplayRenderMode
{
  Partial,
  Direct
};

using DisplayTxDoneCallback = void (*)(void* arg);

class IDisplayDriver
{
public:
  virtual ~IDisplayDriver() = default;

  virtual void init(DisplayTxDoneCallback callback, void* callbackArg) = 0;

  virtual void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData) = 0;

  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual DisplayRenderMode preferredRenderMode() const = 0;

  virtual bool requiresByteSwap() const { return false; }

  virtual void* getFramebuffer(int index) const { return nullptr; }
};

#endif // IDISPLAY_DRIVER_H