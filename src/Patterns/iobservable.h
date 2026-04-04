#ifndef IOBSERVABLE_H
#define IOBSERVABLE_H

template <typename T>
class IObservable
{
public:
  virtual void attachListener(T* observer) = 0;
  virtual void detachListener(T* observer) = 0;
};

#endif // IOBSERVABLE_H