#pragma once

#include <type_traits>

#include <nlohmann_json/single_include/nlohmann/json.hpp>

//template <bool>
//struct TypePlaceHolder;
//
//template <>
//struct TypePlaceHolder<true> {
//  typedef int type;
//};

//template <bool>
//struct FundamentalPlaceHolder;
//
//template <>
//struct FundamentalPlaceHolder<true> {
//  typedef int type;
//};
//
//template <bool>
//struct StdStringPlaceHolder;
//
//template <>
//struct StdStringPlaceHolder<true> {
//  typedef int type;
//};
// 


////fundamental
//template <class T>
//void TypeSerialize(const T& from, nlohmann::json& to 
//  , typename std::enable_if<std::is_fundamental<T>::value, int>::type = {})
//{
//  to = from;
//}
//template <class T>
//void TypeDeserialize(T& to, const nlohmann::json& from
//  , typename std::enable_if<std::is_fundamental<T>::value, int>::type = {})
//{
//  from.template get<T>;
//}

//fundamental
//std::string
template <class T>
void TypeSerialize(const T& from, nlohmann::json& to
  , typename std::enable_if<std::is_fundamental<T>::value
  || std::is_base_of<std::string, T>::value 
  , int>::type = {})
{
  static_assert(
    nlohmann::detail::is_default_constructible<T>::value
    && nlohmann::detail::has_from_json<nlohmann::json, T>::value
    , "not compabiliable");
  to = from;
}

template <class T>
bool IsCompatable(const nlohmann::json& json) {
  switch (json.type()) {
  case nlohmann::json::value_t::string:
    return std::is_base_of<std::string, T>::value;
  case nlohmann::json::value_t::boolean:
    return std::is_arithmetic<T>::value;
  case nlohmann::json::value_t::number_integer:
  case nlohmann::json::value_t::number_unsigned:
    return std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value
      || std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value;
  case nlohmann::json::value_t::number_float:
    return std::is_same<T, float>::value || std::is_same<T, double>::value;
  default:
    return false;
  }
}

template <class T>
void TypeDeserialize(T& to, const nlohmann::json& from
  , typename std::enable_if<
      std::is_fundamental<T>::value, int>::type = {})
{
  static_assert(nlohmann::detail::has_from_json<nlohmann::json, T>::value
    , "not compatable");

  //
  if (!IsCompatable<T>(from)) return;

  from_json(from, to);
}

template <class T>
void TypeDeserialize(T& to, const nlohmann::json& from
  , typename std::enable_if<std::is_base_of<std::string, T>::value, int>::type = {})
{
  if (from.is_string()) from_json(from, to);
  else if (from.is_object() || from.is_array()) {
    to = from.dump();
  }
}

//vector
//std::vector
template <class T>
void TypeSerialize(const std::vector<T>& from, nlohmann::json& to
  /*, typename TypePlaceHolder<std::is_base_of<std::vector<T>, T>::value>::type = {}*/)
{
  if (!to.is_null() && !to.is_array()) return;
  for (const auto& from_it : from) {
    nlohmann::json tmp;
    TypeSerialize(from_it, tmp);
    to.push_back(std::move(tmp));
  }
}

template <class T>
void TypeDeserialize(std::vector<T>& to, const nlohmann::json& from
/*, typename TypePlaceHolder<std::is_base_of<std::vector<T>, T>::value>::type = {}*/)
{
  if (!from.is_array()) return;
  for (const auto& from_it : from) {
    T tmp;
    TypeDeserialize(tmp, from_it);
    to.push_back(std::move(tmp));
  }
}

//others
template <class T>
void TypeSerialize(const T& from, nlohmann::json& to
  , typename std::enable_if<
    !std::is_fundamental<T>::value 
    && !std::is_base_of<std::string, T>::value, int>::type = {})
{
  from.Serialize(to);
}

template <class T>
void TypeDeserialize(T& to, const nlohmann::json& from
  , typename std::enable_if<
  !std::is_fundamental<T>::value
  && !std::is_base_of<std::string, T>::value, int>::type = {})
{
  to.Deserialize(from);
}

