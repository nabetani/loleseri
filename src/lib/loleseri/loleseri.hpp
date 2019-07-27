#pragma once

#include <iterator>
#include <tuple>
#include <type_traits>
#include <array>

namespace loleseri {

template <typename target_type, int typecat> struct serializer_impl;

template< typename target_type >
struct type_category{
  enum{
    value = ( typename std::is_same<bool, target_type>::type() ? 1 : 0)
  };
};

namespace tcat{
  constexpr int boolean = type_category<bool>::value;
  constexpr int arithmetic = type_category<int>::value;
}

template <typename target_type>
using serializer =
    serializer_impl<target_type, type_category<target_type>::value>;
} // namespace loleseri

template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::arithmetic>
{
  using target_type = target_type_;
  using buffer = std::array<std::uint8_t, sizeof(target_type)>;

  template< typename itor_t >
  static
  itor_t serialize( itor_t begin, itor_t end, target_type const * obj )
  {
    auto p = reinterpret_cast<std::uint8_t const*>(obj);
    return std::copy(p, p+sizeof(target_type), begin );
  }
};

template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::boolean>
{
  using target_type = target_type_;
  using buffer = std::array<std::uint8_t, sizeof(target_type)>;

  template< typename itor_t >
  static
  itor_t serialize( itor_t begin, itor_t end, target_type const * obj )
  {
    *begin = *obj ? 1 : 0;
    return std::next(begin);
  }
};
