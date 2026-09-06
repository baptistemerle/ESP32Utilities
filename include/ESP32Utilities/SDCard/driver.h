#pragma once

#include <filesystem>
#include <functional>

#include <esp_err.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"
#include "ESP32Utilities/SDCard/configuration.h"
#include "ESP32Utilities/SDCard/storage_info.h"

namespace esp32utilities::sdcard
{

class Driver
{
protected:
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;
  using PinControlCallback = std::function<void(GPIOLevel level)>;

public:
  Driver(PinControlCallback setCsCallback);
  ~Driver();

  esp_err_t mount(const Configuration& configuration, const std::string& mountPoint = "/sdcard");
  void unmount();

  std::optional<StorageInfo> getStorageInfo() const;

  bool exists(const std::filesystem::path& path) const;

  bool createDirectories(const std::filesystem::path& path);

  bool appendText(const std::filesystem::path& path, std::string_view text);
  bool appendBinary(const std::filesystem::path& path, std::span<const uint8_t> data);

  std::vector<uint8_t> readBinary(const std::filesystem::path& path);
  std::string readText(const std::filesystem::path& path);

private:
  PinControlCallback m_setCsCallback;

  std::filesystem::path m_mountPoint;
  sdmmc_card_t* m_card { nullptr };
  bool m_isMounted { false };
};

} // namespace esp32utilities::sdcard
