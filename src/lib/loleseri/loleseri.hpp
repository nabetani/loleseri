#pragma once

#include <iterator>
#include <tuple>
#include <type_traits>
#include <array>

namespace loleseri {

template <typename target_type, typename arithmetic> struct serializer_impl;

template <typename target_type>
using serializer =
    serializer_impl<target_type, typename std::is_arithmetic<target_type>::type>;

} // namespace loleseri

template <typename target_type_>
struct loleseri::serializer_impl<target_type_, std::true_type>
{
  using target_type = target_type_;
  using buffer = std::array<target_type,1>;

  template< typename itor_t >
  static
  itor_t serialize( itor_t begin, itor_t end, target_type const * obj )
  {
    *begin=*obj;
    return std::next(begin, 1);
  }
};
