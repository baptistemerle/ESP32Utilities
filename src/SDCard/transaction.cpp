#include "transaction.h"

#include "ESP32Utilities/SDCard/volume.h"

namespace esp32utilities::sdcard
{

Transaction::Transaction(const Volume& volume)
  : m_volume(volume)
{
  m_volume.select();
}

Transaction::~Transaction()
{
  m_volume.deselect();
}

} // namespace esp32utilities::sdcard
