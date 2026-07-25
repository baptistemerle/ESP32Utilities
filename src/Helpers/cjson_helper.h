#ifndef CJSON_HELPER_H
#define CJSON_HELPER_H

#include <cJSON.h>
#include <cstdint>
#include <string>

namespace cJsonHelper
{
inline bool extract(const cJSON* item, bool& out)
{
  if (item && cJSON_IsBool(item))
  {
    out = cJSON_IsTrue(item);
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, int& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = item->valueint;
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, uint8_t& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = static_cast<uint8_t>(item->valueint);
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, uint16_t& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = static_cast<uint16_t>(item->valueint);
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, uint32_t& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = static_cast<uint32_t>(item->valueint);
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, float& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = static_cast<float>(item->valuedouble);
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, double& out)
{
  if (item && cJSON_IsNumber(item))
  {
    out = item->valuedouble;
    return true;
  }

  return false;
}

inline bool extract(const cJSON* item, std::string& out)
{
  if (item && cJSON_IsString(item) && item->valuestring)
  {
    out = item->valuestring;
    return true;
  }

  return false;
}

template <typename ValueType, typename TargetType>
bool read(const cJSON* root, const char* key, TargetType& target)
{
  if (!root)
    return false;

  cJSON* item = cJSON_GetObjectItemCaseSensitive(root, key);

  ValueType temp{};
  if (extract(item, temp))
  {
    target = temp;
    return true;
  }

  return false;
}

template <typename TargetType>
bool read(const cJSON* root, const char* key, TargetType& target)
{
  return read<TargetType, TargetType>(root, key, target);
}

template <typename ValueType, typename TargetType, typename Transform>
bool read(const cJSON* root, const char* key, TargetType& target, Transform transform)
{
  if (!root)
    return false;

  cJSON* item = cJSON_GetObjectItemCaseSensitive(root, key);

  ValueType temp{};
  if (extract(item, temp))
  {
    target = transform(temp);
    return true;
  }

  return false;
}

}

#endif // CJSON_HELPER_H