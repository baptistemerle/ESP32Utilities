#pragma once

#include <filesystem>
#include <span>
#include <string_view>
#include <vector>

namespace esp32utilities::sdcard
{

class Volume;

class FileSystem
{
public:
  explicit FileSystem(const Volume& volume);

  bool exists(const std::filesystem::path& path) const;

  bool createDirectories(const std::filesystem::path& path);

  bool appendText(const std::filesystem::path& path, std::string_view text);
  bool appendBinary(const std::filesystem::path& path, std::span<const uint8_t> data);

  std::vector<uint8_t> readBinary(const std::filesystem::path& path) const;
  std::string readText(const std::filesystem::path& path) const;

private:
  const Volume& m_volume;
};

} // namespace esp32utilities::sdcard
