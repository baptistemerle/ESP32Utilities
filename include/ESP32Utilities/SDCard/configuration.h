#pragma once

#include <driver/sdspi_host.h>
#include <esp_vfs_fat.h>

namespace esp32utilities::sdcard
{

struct Configuration
{
  sdmmc_host_t               hostConfig;
  sdspi_device_config_t      slotConfig;
  esp_vfs_fat_mount_config_t mountConfig;
};

} // namespace esp32utilities::sdcard
