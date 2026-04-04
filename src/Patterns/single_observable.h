#ifndef SINGLE_OBSERVABLE_H
#define SINGLE_OBSERVABLE_H

#include "iobservable.h"

#include <utility>

template <typename T>
class SingleObservable : public IObservable<T>
{
public:
  virtual void attachListener(T* observer) override
  {
    m_observer = observer;
  }

  virtual void detachListener(T* observer) override
  {
    if (m_observer != observer)
      return;

    m_observer = nullptr;
  }

protected:
  template <typename Function, typename... Args>
  void notify(Function function, Args&&... args)
  {
    if (m_observer)
    {
      (m_observer->*function)(std::forward<Args>(args)...);
    }
  }

private:
  T* m_observer = nullptr;
};

#endif // SINGLE_OBSERVABLE_H