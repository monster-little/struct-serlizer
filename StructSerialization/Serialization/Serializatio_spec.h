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
void TypeDeserialize(T& to, const nlohmann::json& from
  , typename std::enable_if<
      std::is_fundamental<T>::value || std::is_base_of<std::string, T>::value
  , int>::type = {})
{
  static_assert(nlohmann::detail::has_from_json<nlohmann::json, T>::value
    , "not compabiliable");
  from_json(from, to);
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

