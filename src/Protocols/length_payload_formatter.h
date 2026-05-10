#ifndef LENGTH_PAYLOAD_FORMATTER_H
#define LENGTH_PAYLOAD_FORMATTER_H

#include <string>
#include <sstream>
#include <iomanip>

template <size_t HeaderSize>
class LengthPayloadFormatter
{
public:
  static std::string format(const std::string& payload)
  {
    std::ostringstream oss;

    oss << std::setw(HeaderSize) << std::setfill('0') << payload.size();

    oss << payload;

    return oss.str();
  }
};

#endif // LENGTH_PAYLOAD_FORMATTER_H
