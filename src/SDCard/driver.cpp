#include "ESP32Utilities/SDCard/driver.h"
#include "ESP32Utilities/SDCard/configuration.h"

#include <fstream>
#include <sys/statvfs.h>

namespace esp32utilities::sdcard
{

class CsGuard
{
protected:
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;
  using PinControlCallback = std::function<void(GPIOLevel level)>;

public:
  explicit CsGuard(PinControlCallback setCsCallback)
    : m_setCsCallback(setCsCallback)
  {
    if (m_setCsCallback)
      m_setCsCallback(GPIOLevel::Low);
  }

  ~CsGuard()
  {
    if (m_setCsCallback)
      m_setCsCallback(GPIOLevel::High);
   }

private:
  PinControlCallback m_setCsCallback;
};

Driver::Driver(PinControlCallback setCsCallback)
  : m_setCsCallback(setCsCallback)
{
}

Driver::~Driver()
{
  unmount();
}

esp_err_t Driver::mount(const Configuration& configuration, const std::string& mountPoint)
{
  if (m_isMounted)
    return ESP_ERR_INVALID_STATE;

  m_mountPoint = mountPoint;

  CsGuard guard(m_setCsCallback);

  esp_err_t ret = esp_vfs_fat_sdspi_mount(m_mountPoint.c_str(), &configuration.hostConfig, &configuration.slotConfig, &configuration.mountConfig, &m_card);

  m_isMounted = (ret == ESP_OK);

  return ret;
}

void Driver::unmount()
{
  if (m_isMounted && m_card)
  {
    CsGuard guard(m_setCsCallback);

    esp_vfs_fat_sdcard_unmount(m_mountPoint.c_str(), m_card);

    m_card = nullptr;
    m_isMounted = false;
  }
}

std::optional<StorageInfo> Driver::getStorageInfo() const
{
  if (!m_isMounted)
    return std::nullopt;

  CsGuard guard(m_setCsCallback);

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

bool Driver::exists(const std::filesystem::path& path) const
{
  if (!m_isMounted)
    return false;

 CsGuard guard(m_setCsCallback);

  std::error_code ec;
  bool result = std::filesystem::exists(m_mountPoint / path, ec);

  return !ec && result;
}

bool Driver::createDirectories(const std::filesystem::path& path)
{
  if (!m_isMounted)
    return false;

  CsGuard guard(m_setCsCallback);

  std::error_code ec;
  bool result = std::filesystem::create_directories(m_mountPoint / path, ec);

  return !ec && result;
}

bool Driver::appendText(const std::filesystem::path& path, std::string_view text)
{
  if (!m_isMounted)
    return false;

  CsGuard guard(m_setCsCallback);

  std::ofstream file(m_mountPoint / path, std::ios::app);
  if (!file.is_open())
  {
    return false;
  }

  file << text;
  file.flush();
  file.close();

  return file.good();
}

bool Driver::appendBinary(const std::filesystem::path& path, std::span<const uint8_t> data)
{
  if (!m_isMounted)
    return false;

  CsGuard guard(m_setCsCallback);

  std::ofstream file(m_mountPoint / path, std::ios::app | std::ios::binary);
  if (!file.is_open())
  {
    return false;
  }
  file.write(reinterpret_cast<const char*>(data.data()), data.size());
  file.flush();
  file.close();

  return file.good();
}

std::vector<uint8_t> Driver::readBinary(const std::filesystem::path& path)
{
  if (!m_isMounted)
    return {};

  CsGuard guard(m_setCsCallback);

  std::ifstream file(m_mountPoint / path, std::ios::binary | std::ios::ate);
  if (!file.is_open())
  {
    return {};
  }

  auto fileSize = file.tellg();
  if (fileSize <= 0)
  {
    return {};
  }

  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(fileSize);
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

  return buffer;
}

std::string Driver::readText(const std::filesystem::path& path)
{
  if (!m_isMounted)
    return {};

  CsGuard guard(m_setCsCallback);

  std::ifstream file(m_mountPoint / path, std::ios::ate);
  if (!file.is_open())
  {
    return {};
  }

  auto fileSize = file.tellg();

  if (fileSize <= 0)
  {
      return {};
  }

  file.seekg(0, std::ios::beg);

  std::string content;
  content.resize(fileSize);

  file.read(content.data(), fileSize);

  return content;
}

} // namespace esp32utilities::sdcard
