#pragma once

#include <string>
#include <vector>

#include <nlohmann_json/single_include/nlohmann/json.hpp>

#include "Serializatio_spec.h"

class SerializationBase {
protected:
  using OffserType = long long;
  static_assert(sizeof(OffserType) >= sizeof(void*), "pointer size check");
  struct SerializationInfo {
    typedef void (*Callable)(nlohmann::json &dst, const void* mem);
    std::string key;
    Callable fun;
    OffserType mem_offset;
    OffserType base_offset;
  };

  struct DeserializationInfo {
    typedef void (*Callable)(const nlohmann::json& origin, void* mem);
    std::string key;
    Callable fun;
    OffserType mem_offset;
    OffserType base_offset;
  };

  using DSIList = std::vector<DeserializationInfo>;
  using SIList = std::vector<SerializationInfo>;

  
  struct DSInserter {
    template<class SelfType, class MemType>
    DSInserter(DSIList* list, const std::string& key,
      DeserializationInfo::Callable method, SelfType* self, MemType* member)
    {
      if (!list) return;
      list->emplace_back(DeserializationInfo{
        key, method, (OffserType)member - (OffserType)self, (OffserType)(SerializationBase*)(SelfType*)0
        });

    }
  };
  struct SInserter {
    template<class SelfType, class MemType>
    SInserter(SIList* list, const std::string& key,
      SerializationInfo::Callable method, SelfType* self, MemType* member)
    {
      if (!list) return;
      list->emplace_back(SerializationInfo{
        key, method, (OffserType)member - (OffserType)self, (OffserType)(SerializationBase*)(SelfType*)0
        });
    }
  };

protected:
  ~SerializationBase() {};

public:
  void Serialize(nlohmann::json& buf) const {
    if (!buf.is_null() && !buf.is_object()) return;
    for (const auto& s_i : ser_list_) {
      if (s_i.key.empty()) continue;
      void* mem_addr = (void*)((OffserType)this - s_i.base_offset + s_i.mem_offset);
      auto& v = buf[s_i.key];
      s_i.fun(v, mem_addr);
    }
  }
  void Deserialize(const nlohmann::json& buf)
  {
    if (!buf.is_null() && !buf.is_object()) return;
    for (const auto& s_i : deser_list_) {
      if (s_i.key.empty()) continue;
      void* mem_addr = (void*)((OffserType)this - s_i.base_offset + s_i.mem_offset);
      auto itr = buf.find(s_i.key);
      if (itr == buf.cend()) continue;
      const auto& v = *itr;
      s_i.fun(v, mem_addr);
    }
  }

protected:
  SIList ser_list_;
  DSIList deser_list_;
};

#define SERIALIZATION_MEMBER(type, name) \
public: \
  type name; \
private: \
  static void Serialize_##name(nlohmann::json& dst, const void* mem) \
  { \
    if (!mem) return; \
    const type * val_addr = (const type * )mem;\
    TypeSerialize(*val_addr, dst); \
  } \
  static void Deserialize_##name(const nlohmann::json& from, void* mem) \
  { \
    if (!mem) return; \
    type * val_addr = (type * )mem;\
    TypeDeserialize(*val_addr, from); \
  } \
   \
  SInserter name##s_inserter_ = {&ser_list_, #name, &this->Serialize_##name, this, &name};\
  DSInserter name##ds_inserter_ = {&deser_list_, #name, &this->Deserialize_##name, this, &name};



