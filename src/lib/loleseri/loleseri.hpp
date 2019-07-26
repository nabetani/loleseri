#pragma once

#include <tuple>

namespace loleseri
{
  template<typename type>
  struct items;


  template <typename... args>
  struct tuple_item_sum;

  template< typename sometype >
  struct ptr_to_member;

  template< typename struct_t, typename member_t >
  struct ptr_to_member<member_t struct_t::*>
  {
    static constexpr inline size_t size(){  return sizeof(member_t);  }
  };

  template <typename arg0, typename... args>
  struct tuple_item_sum<std::tuple<arg0, args...>>{
    static constexpr size_t size(){  
      return ptr_to_member<arg0>::size() + tuple_item_sum<std::tuple<args...>>::size();
    }
  };

  template <>
  struct tuple_item_sum<std::tuple<>>{
    static constexpr size_t size(){  
      return 0;
    }
  };

  template<typename type>
  constexpr
  size_t size()
  {
    using list_type = typename items<type>::list_type;
    return tuple_item_sum<list_type>::size();
  }
}

