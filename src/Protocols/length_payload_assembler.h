#ifndef LENGTH_PAYLOAD_ASSEMBLER_H
#define LENGTH_PAYLOAD_ASSEMBLER_H

#include "imessage_assembler.h"

/*
 * Reassembles fragmented incoming data using a length-prefixed protocol.
 * The protocol requires the first 4 bytes to represent the payload length (zero-padded),
 * immediately followed by the raw data.
 *
 * * Frame format example: 0011{"data":12}
 * -> Header: 0011 (4 bytes)
 * -> Payload: {"data":12} (11 bytes)
 */
template <size_t HeaderSize>
class LengthPayloadAssembler : public IMessageAssembler
{
public:
  LengthPayloadAssembler(PayloadCallback callback)
    : IMessageAssembler(callback)
  {}

  virtual void pushData(const std::string& value) override
  {
    m_incomingBuffer += value;

    bool process = true;
    while (process)
    {
      if (m_expectedLength == 0)
      {
        extractLength();
      }

      if (m_incomingBuffer.length() > 0 && m_expectedLength > 0)
      {
        if (m_incomingBuffer.length() >= m_expectedLength)
        {
          std::string payload = m_incomingBuffer.substr(0, m_expectedLength);

          if (m_onMessageReady)
          {
            m_onMessageReady(payload);
          }

          m_incomingBuffer.erase(0, m_expectedLength);
          m_expectedLength = 0;
        }
        else
        {
          process = false;
        }
      }
      else
      {
        process = false;
      }
    }
  }

private:
  bool isHeaderValid(const std::string& header)
  {
    for (char c : header)
    {
      if (!isdigit(c))
      {
        return false;
      }
    }

    return true;
  }

  void extractLength()
  {
    while (m_incomingBuffer.length() >= HeaderSize)
    {
      std::string header = m_incomingBuffer.substr(0, HeaderSize);

      if (isHeaderValid(header))
      {
        m_expectedLength = std::atoi(header.c_str());
        m_incomingBuffer.erase(0, HeaderSize);

        return;
      }
      else
      {
        m_incomingBuffer.erase(0, 1);
      }
    }
  }

private:
  size_t m_expectedLength = 0;
  std::string m_incomingBuffer = "";
};

#endif // LENGTH_PAYLOAD_ASSEMBLER_H