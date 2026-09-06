#pragma once

#include <filesystem>
#include <functional>
#include <optional>

#include <esp_err.h>

#include "ESP32Utilities/Interfaces/gpio_types.h"
#include "ESP32Utilities/SDCard/configuration.h"
#include "ESP32Utilities/SDCard/storage_info.h"

namespace esp32utilities::sdcard
{

class Volume
{
  friend class Transaction;

protected:
  using GPIOLevel = esp32utilities::interfaces::GPIO::Level;
  using PinControlCallback = std::function<void(GPIOLevel level)>;

public:
  Volume(const Configuration& configuration, PinControlCallback setCsCallback);
  ~Volume();

  Volume(const Volume&) = delete;
  Volume& operator=(const Volume&) = delete;

  esp_err_t mount(std::string_view mountPoint = "/sdcard");
  void unmount();

  bool isMounted() const;
  const std::filesystem::path& mountPoint() const;

  std::optional<StorageInfo> getStorageInfo() const;

private:
  void select() const;
  void deselect() const;

private:
  Configuration m_configuration;
  PinControlCallback m_setCsCallback;

  std::filesystem::path m_mountPoint;
  sdmmc_card_t* m_card { nullptr };
  bool m_isMounted { false };
};

} // namespace esp32utilities::sdcard
