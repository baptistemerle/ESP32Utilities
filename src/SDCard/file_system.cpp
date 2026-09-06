#include "ESP32Utilities/SDCard/file_system.h"

#include <fstream>

#include "ESP32Utilities/SDCard/volume.h"

#include "transaction.h"

namespace esp32utilities::sdcard
{

FileSystem::FileSystem(const Volume& volume)
  : m_volume(volume)
{
}

bool FileSystem::exists(const std::filesystem::path& path) const
{
  if (!m_volume.isMounted())
    return false;

  Transaction transaction(m_volume);

  std::error_code ec;
  bool result = std::filesystem::exists(m_volume.mountPoint() / path, ec);

  return !ec && result;
}

bool FileSystem::createDirectories(const std::filesystem::path& path)
{
  if (!m_volume.isMounted())
    return false;

  Transaction transaction(m_volume);

  std::error_code ec;
  bool result = std::filesystem::create_directories(m_volume.mountPoint() / path, ec);

  return !ec && result;
}

bool FileSystem::appendText(const std::filesystem::path& path, std::string_view text)
{
  if (!m_volume.isMounted())
    return false;

  Transaction transaction(m_volume);

  std::ofstream file(m_volume.mountPoint() / path, std::ios::app);
  if (!file.is_open())
  {
    return false;
  }

  file << text;
  file.flush();
  file.close();

  return file.good();
}

bool FileSystem::appendBinary(const std::filesystem::path& path, std::span<const uint8_t> data)
{
  if (!m_volume.isMounted())
    return false;

  Transaction transaction(m_volume);

  std::ofstream file(m_volume.mountPoint() / path, std::ios::app | std::ios::binary);
  if (!file.is_open())
  {
    return false;
  }

  file.write(reinterpret_cast<const char*>(data.data()), data.size());
  file.flush();
  file.close();

  return file.good();
}

std::vector<uint8_t> FileSystem::readBinary(const std::filesystem::path& path) const
{
  if (!m_volume.isMounted())
    return {};

  Transaction transaction(m_volume);

  std::ifstream file(m_volume.mountPoint() / path, std::ios::binary | std::ios::ate);
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

std::string FileSystem::readText(const std::filesystem::path& path) const
{
  if (!m_volume.isMounted())
    return {};

  Transaction transaction(m_volume);

  std::ifstream file(m_volume.mountPoint() / path, std::ios::ate);
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
