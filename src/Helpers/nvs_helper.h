#ifndef NVS_HELPER_H
#define NVS_HELPER_H

#include <esp_check.h>
#include <nvs.h>

static constexpr char TAG[] = "NVSHelper";

namespace NvsHelper
{
inline esp_err_t read(nvs_handle_t handle, const char* key, uint8_t& val)  { return nvs_get_u8(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int8_t& val)   { return nvs_get_i8(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, uint16_t& val) { return nvs_get_u16(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int16_t& val)  { return nvs_get_i16(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, uint32_t& val) { return nvs_get_u32(handle, key, &val); }
inline esp_err_t read(nvs_handle_t handle, const char* key, int32_t& val)  { return nvs_get_i32(handle, key, &val); }

inline esp_err_t write(nvs_handle_t handle, const char* key, uint8_t val)  { return nvs_set_u8(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int8_t val)   { return nvs_set_i8(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, uint16_t val) { return nvs_set_u16(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int16_t val)  { return nvs_set_i16(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, uint32_t val) { return nvs_set_u32(handle, key, val); }
inline esp_err_t write(nvs_handle_t handle, const char* key, int32_t val)  { return nvs_set_i32(handle, key, val); }

template <typename NvsType, typename TargetType, typename Transform>
void loadParam(nvs_handle_t nvsHandle, const char* key, NvsType defaultValue, TargetType& target, Transform transform)
{
  NvsType savedValue = defaultValue;
  esp_err_t err = read(nvsHandle, key, savedValue);

  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    ESP_LOGE(TAG, "Failed to load key: %s (err: %s)", key, esp_err_to_name(err));
  }

  printf("NVS loaded [%s]: %lld\n", key, static_cast<long long>(savedValue));

  target = transform(savedValue);
}

template <typename T>
void loadParam(nvs_handle_t nvsHandle, const char* key, T defaultValue, T& target)
{
  loadParam(nvsHandle, key, defaultValue, target, [](T val) { return val; });
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
      printf("NVS saved [%s]: %lld\n", key, static_cast<long long>(valueToSave));
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

}

#endif // NVS_HELPER_H