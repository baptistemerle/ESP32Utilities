#include "ESP32Utilities/SDCard/volume.h"

#include "ESP32Utilities/SDCard/configuration.h"

#include "transaction.h"

namespace esp32utilities::sdcard
{

Volume::Volume(const Configuration& configuration, PinControlCallback setCsCallback)
  : m_configuration(configuration)
  , m_setCsCallback(setCsCallback)
{
}

Volume::~Volume()
{
  unmount();
}

esp_err_t Volume::mount(std::string_view mountPoint)
{
  if (m_isMounted)
    return ESP_ERR_INVALID_STATE;

  m_mountPoint = mountPoint;

  Transaction transaction(*this);

  esp_err_t ret = esp_vfs_fat_sdspi_mount(m_mountPoint.c_str(), &m_configuration.hostConfig, &m_configuration.slotConfig, &m_configuration.mountConfig, &m_card);

  m_isMounted = (ret == ESP_OK);

  return ret;
}

void Volume::unmount()
{
  if (m_isMounted && m_card)
  {
    Transaction transaction(*this);

    esp_vfs_fat_sdcard_unmount(m_mountPoint.c_str(), m_card);

    m_card = nullptr;
    m_isMounted = false;
  }
}

bool Volume::isMounted() const
{
  return m_isMounted;
}

const std::filesystem::path& Volume::mountPoint() const
{
  return m_mountPoint;
}

std::optional<StorageInfo> Volume::getStorageInfo() const
{
  if (!m_isMounted)
    return std::nullopt;

  Transaction transaction(*this);

  uint64_t totalBytes = 0;
  uint64_t freeBytes = 0;

  esp_err_t ret = esp_vfs_fat_info(m_mountPoint.c_str(), &totalBytes, &freeBytes);
  if (ret != ESP_OK)
  {
    return std::nullopt;
  }

  StorageInfo info;
  info.totalBytes = totalBytes;
  info.freeBytes  = freeBytes;
  info.usedBytes  = totalBytes - freeBytes;

  return info;
}

void Volume::select() const
{
  if (m_setCsCallback)
    m_setCsCallback(GPIOLevel::Low);
}

void Volume::deselect() const
{
  if (m_setCsCallback)
    m_setCsCallback(GPIOLevel::High);
}

} // namespace esp32utilities::sdcard
