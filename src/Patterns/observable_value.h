#ifndef OBSERVABLE_VALUE_H
#define OBSERVABLE_VALUE_H

#include <functional>
#include <vector>

template <typename T>
class ObservableValue
{
public:
    using Callback = std::function<void(const T&)>;

    ObservableValue(T initialValue = T{})
      : m_value(initialValue)
    {}

    void attach(Callback callback)
    {
      m_callbacks.push_back(callback);
    }

    /*
     * Note on observer detachment (detach method):
     * In this embedded system architecture, observers (such as UI widgets or storage controllers)
     * are instantiated at boot and live throughout the entire application lifecycle.
     * Since dynamic subscription/unsubscription is not required, a `detach()` method has been
     * intentionally omitted. This avoids the overhead and complexity of managing unique IDs,
     * as `std::function` objects cannot be compared directly in C++.
     * If needed, we could implement a token system
     */

    operator T() const
    {
      return m_value;
    }

    ObservableValue& operator=(const T& newValue)
    {
      if (m_value != newValue)
      {
        m_value = newValue;

        for (const auto& callback : m_callbacks)
        {
          if (callback)
          {
            callback(m_value);
          }
        }
      }

      return *this;
    }

private:
    T m_value;
    std::vector<Callback> m_callbacks;
};

#endif // OBSERVABLE_VALUE_H