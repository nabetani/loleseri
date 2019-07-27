#pragma once

#include <array>
#include <iterator>
#include <loleseri/endian.hpp>
#include <tuple>
#include <type_traits>

/** low level serializer */
namespace loleseri {

/** template to count items in tuple */
template <typename... args> struct tuple_item_count;

/** template to count items in tuple */
template <typename... args> struct tuple_item_count<std::tuple<args...>> {
  enum { value = sizeof...(args) };
};

/** template to specify how to serialize
 * @tparam t target type.
 */
template <typename t> struct items;

/** template to serialize
 * @tparam target_type target type
 * @tparam typecat integer to specity category of target type
 */
template <typename target_type, int typecat> struct serializer_impl;

/** template to calculate category value of target type
 * @tparam target_type calculate category value of this type
 */
template <typename target_type> struct type_category {
  enum {
    /** category value */
    value = (typename std::is_same<bool, target_type>::type() ? 1 : 0) +
            (typename std::is_arithmetic<target_type>::type() ? 2 : 0)
  };
};

/** hepler template to calculate byte count of serialized binary
 * @tparam item_type type to specify serialized binary size
 */
template <typename item_type> struct item_size;

/** hepler template to calculate byte count of serialized binary
 * @tparam value_type type of the value to serialize
 * @tparam owner_type type of the owner of the value
 */
template <typename value_type, typename owner_type>
struct item_size<value_type owner_type::*> {
  enum { value = sizeof(value_type) };
};

/** template to calculate serialized binary size */
template <typename... args> struct serialized_size;

/** template to calculate serialized binary size
 * @tparam arg0 type of the first item in tuple
 * @tparam args types of the rests
 */
template <typename arg0, typename... args>
struct serialized_size<std::tuple<arg0, args...>> {
  enum {
    /** serialized binary size in bytes */
    value = item_size<arg0>::value + serialized_size<std::tuple<args...>>::value
  };
};

/** template to calculate serialized binary size */
template <> struct serialized_size<std::tuple<>> {
  enum {
    /** serialized binary size in bytes */
    value = 0
  };
};

/** values to specify category of the type */
namespace tcat {

/** this value means bool */
constexpr int boolean = type_category<bool>::value;

/** this value means "integer or floating point value but not bool" */
constexpr int arithmetic = type_category<int>::value;
struct structure {};

/** this value means "not bool, not arithetic" */
constexpr int other = type_category<structure>::value;
} // namespace tcat

/** type to serialize target type
 * @tparam target_type type of the value to serialize
 */
template <typename target_type>
using serializer =
    serializer_impl<target_type, type_category<target_type>::value>;
} // namespace loleseri

/** type to serialize integer or floating point type
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::arithmetic> {

  /** type of the value to serialize */
  using target_type = target_type_;

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, sizeof(target_type)>;

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
#if LOLESERI_LITTLE_ENDIAN
    auto p = reinterpret_cast<std::uint8_t const *>(obj);
    return std::copy(p, p + sizeof(target_type), begin);
#else
#error "you should write something here."
#endif
  }
};

/** type to serialize bool
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::boolean> {
  /** type of the value to serialize */
  using target_type = target_type_;

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, sizeof(target_type)>;

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
    *begin = *obj ? 1 : 0;
    return std::next(begin);
  }
};

/** type to serialize struct or class
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::other> {
  /** type of the value to serialize */
  using target_type = target_type_;

  /** type to get list of items to serialize */
  using items = loleseri::items<target_type>;

  /** type of the list of items to serialize */
  using list_type = decltype(items::list());

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, serialized_size<list_type>::value>;

  /** template to serialize part of struct or class
   * @tparam ix skip first ix items
   * @tparam bool true if nothing to do more
   */
  template <size_t ix, bool end_of_tuple> struct partial_serializer;

  /** template to terminate serialization
   * @tparam ix skip first ix items
   */
  template <size_t ix> struct partial_serializer<ix, true> {
    /** returns begin ( there is nothing to serialize )
     * @param[in] begin top of the output iterator
     * @param[in] end end of the output iterator
     * @param[in] obj pointer to the object to serialize
     * @return iterator which points to the begin of the unused area
     */
    template <typename itor_t>
    static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
      return begin;
    }
  };

  /** template to serialize part of struct or class
   * @tparam ix skip first ix items
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <size_t ix> struct partial_serializer<ix, false> {
    /** serialize obj to output iterator
     * @tparam itor_t type of the output iterator
     * @param[in] begin top of the output iterator
     * @param[in] end end of the output iterator
     * @param[in] obj pointer to the object to serialize
     * @return iterator which points to the begin of the unused area
     */
    template <typename itor_t>
    static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
      constexpr size_t tc = tuple_item_count<list_type>::value;
      auto m = std::get<ix>(items::list());
      using item_type = typename std::remove_reference<decltype(obj->*m)>::type;
      using seri = loleseri::serializer<item_type>;
      auto p = seri::serialize(begin, end, &(obj->*m));
      return partial_serializer<ix + 1, (tc <= ix + 1)>::serialize(p, end, obj);
    }
  };

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
    constexpr size_t tc = tuple_item_count<list_type>::value;
    return partial_serializer<0, (tc <= 0)>::serialize(begin, end, obj);
  }
};
