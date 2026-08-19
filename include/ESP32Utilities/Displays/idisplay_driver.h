#pragma once

#include <cstdint>

namespace esp32utilities::displays
{

enum class DisplayRenderMode
{
  Partial,
  Direct
};

class IDisplayDriver
{
protected:
  using DisplayTxDoneCallback = void (*)(void* arg);

public:
  virtual ~IDisplayDriver() = default;

  virtual void init(DisplayTxDoneCallback callback, void* callbackArg) = 0;

  virtual void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const void* rawData) = 0;

  virtual uint32_t width() const = 0;
  virtual uint32_t height() const = 0;
  virtual DisplayRenderMode preferredRenderMode() const = 0;

  virtual bool requiresByteSwap() const { return false; }

  virtual void* getFrameBuffer(uint8_t index) const { return nullptr; }
};

} // namespace esp32utilities::displays
