#pragma once

namespace esp32utilities::patterns
{

template <typename T>
class IObservable
{
public:
  virtual void attachListener(T* observer) = 0;
  virtual void detachListener(T* observer) = 0;
};

} // namespace esp32utilities::patterns
