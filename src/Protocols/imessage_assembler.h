#ifndef IMESSAGE_ASSEMBLER_H
#define IMESSAGE_ASSEMBLER_H

#include <functional>

class IMessageAssembler
{
public:
  using PayloadCallback = std::function<void(const std::string&)>;

public:
  IMessageAssembler(PayloadCallback onMessageReady)
    : m_onMessageReady(onMessageReady)
  {}

  virtual void pushData(const std::string& value) = 0;

protected:
  PayloadCallback m_onMessageReady;
};

#endif // IMESSAGE_ASSEMBLER_H