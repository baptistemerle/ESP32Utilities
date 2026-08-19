#pragma once

#include <string>

#include <esp_check.h>
#include <nvs.h>

namespace esp32utilities::helpers::nvs
{

static constexpr char TAG[] = "NVSHelper";

inline esp_err_t read(nvs_handle_t handle, const char* key, uint8_t& val)  { return nvs_get_u8(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int8_t& val)   { return nvs_get_i8(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, uint16_t& val) { return nvs_get_u16(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int16_t& val)  { return nvs_get_i16(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, uint32_t& val) { return nvs_get_u32(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int32_t& val)  { return nvs_get_i32(handle, key, &val); }

inline esp_err_t read(nvs_handle_t handle, const char* key, std::string& val)
{
  size_t required_size = 0;
  esp_err_t err = nvs_get_str(handle, key, nullptr, &required_size);
  if (err != ESP_OK)
    return err;

  if (required_size <= 1)
  {
    val.clear();
    return ESP_OK;
  }

  val.resize(required_size - 1);
  return nvs_get_str(handle, key, &val[0], &required_size);
}

inline esp_err_t write(nvs_handle_t handle, const char* key, uint8_t val)  { return nvs_set_u8(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int8_t val)   { return nvs_set_i8(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, uint16_t val) { return nvs_set_u16(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int16_t val)  { return nvs_set_i16(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, uint32_t val) { return nvs_set_u32(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int32_t val)  { return nvs_set_i32(handle, key, val); }

inline esp_err_t write(nvs_handle_t handle, const char* key, const std::string& val) { return nvs_set_str(handle, key, val.c_str()); }
inline esp_err_t write(nvs_handle_t handle, const char* key, const char* val)        { return nvs_set_str(handle, key, val); }

template <typename T>
inline void logValue(const char* action, const char* key, const T& val)
{
  if constexpr (std::is_same_v<T, std::string>)
  {
    printf("%s %s: %s\n", action, key, val.c_str());
  }
  else if constexpr (std::is_same_v<T, const char*>)
  {
    printf("%s %s: %s\n", action, key, val);
  }
  else
  {
    printf("%s %s: %lld\n", action, key, static_cast<long long>(val));
  }
}

template <typename NvsType, typename TargetType, typename Transform>
void loadParam(nvs_handle_t nvsHandle, const char* key, NvsType defaultValue, TargetType& target, Transform transform)
{
  NvsType savedValue = defaultValue;
  esp_err_t err = read(nvsHandle, key, savedValue);

  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    ESP_LOGE(TAG, "Failed to load key: %s (err: %s)", key, esp_err_to_name(err));
  }

  logValue("NVS loaded", key, savedValue);

  target = transform(savedValue);
}

template <typename NvsType, typename TargetType>
void loadParam(nvs_handle_t nvsHandle, const char* key, NvsType defaultValue, TargetType& target)
{
  loadParam(nvsHandle, key, defaultValue, target, [](const NvsType& val) -> const NvsType& { return val; });
}

template <typename NvsType, typename Transform>
auto makeNvsWriter(nvs_handle_t nvsHandle, const char* key, Transform transform)
{
  return [nvsHandle, key, transform](const auto& newValue)
  {
    NvsType valueToSave = transform(newValue);
    esp_err_t err = write(nvsHandle, key, valueToSave);
    if (err == ESP_OK)
    {
      logValue("NVS saved", key, valueToSave);
      nvs_commit(nvsHandle);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to save key: %s (err: %s)", key, esp_err_to_name(err));
    }
  };
}

template <typename NvsType>
auto makeNvsWriter(nvs_handle_t nvsHandle, const char* key)
{
  return makeNvsWriter<NvsType>(nvsHandle, key, [](const auto& val) { return static_cast<NvsType>(val); });
}

} // namespace esp32utilities::helpers::nvs
