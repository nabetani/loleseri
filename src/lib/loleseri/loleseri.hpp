#pragma once

#include <array>
#include <iterator>
#include <loleseri/endian.hpp>
#include <tuple>
#include <type_traits>

/** low level serializer */
namespace loleseri {

/** alternative to  std::size */
template <class T, std::size_t N>
constexpr std::size_t size_of_array(const T (&array)[N]) noexcept {
  return N;
}

template <typename array_type> struct element_type_of_array;

template <typename element, std::size_t N>
struct element_type_of_array<element[N]> {
  using type = element;
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

/** template to deserialize
 * @tparam target_type target type
 * @tparam typecat integer to specity category of target type
 */
template <typename target_type, int typecat> struct deserializer_impl;

/** template to specify type is std::array or not
 * @tparam type target type
 */
template <class type> struct is_std_array : public std::false_type {};

/** template to specify type is std::array or not
 * @tparam type target type
 */
template <class type, size_t size>
struct is_std_array<std::array<type, size>> : public std::true_type {};

/** template to calculate category value of target type
 * @tparam target_type calculate category value of this type
 */
template <typename target_type> struct type_category {
  enum {
    /** category value */
    value = (typename std::is_same<bool, target_type>::type() ? 1 : 0) +
            (typename std::is_arithmetic<target_type>::type() ? 2 : 0) +
            (typename is_std_array<target_type>::type() ? 4 : 0) +
            (typename std::is_array<target_type>::type() ? 8 : 0)
  };
};

/** values to specify category of the type */
namespace tcat {

/** this value means bool */
constexpr int boolean = type_category<bool>::value;

/** this value means "integer or floating point value but not bool" */
constexpr int arithmetic = type_category<int>::value;

/** this value means "std::array" */
constexpr int std_array = type_category<std::array<int, 1>>::value;

/** this value means "traditional array" */
constexpr int array = type_category<int[1]>::value;

/** type to create constant "other" */
struct structure {};

/** this value means "not bool, not arithetic" */
constexpr int other = type_category<structure>::value;
} // namespace tcat

/** type to deserialize target type
 * @tparam target_type type of the value to serialize
 */
template <typename target_type>
using deserializer =
    deserializer_impl<target_type, type_category<target_type>::value>;

/** type to serialize target type
 * @tparam target_type type of the value to serialize
 */
template <typename target_type>
using serializer =
    serializer_impl<target_type, type_category<target_type>::value>;

/** template to get data type from pointer to data member */
template <typename memptr> struct memptr_value;

/** template to get data type from pointer to data member */
template <typename value, typename owner> struct memptr_value<value owner::*> {
  /** data type */
  using type = value;
};

/** type that calculates the sum of the sizes of values pointed to by template
 * member tuple types
 * @tparam tuple_type target type
 */
template <typename tuple_type> struct sum_of_size;

/** type that calculates the sum of the sizes of values pointed to by template
 * member tuple types
 * @tparam arg0 first tuple member type
 * @tparam args rest of tuple member types
 */
template <typename arg0, typename... args>
struct sum_of_size<std::tuple<arg0, args...>> {

  /** data type of arg0 */
  using arg0_value_type = typename memptr_value<arg0>::type;
  enum {
    value = serializer<arg0_value_type>::size +
            sum_of_size<std::tuple<args...>>::value
  };
};

/** type that calculates the sum of the sizes of values pointed to by template
 * member tuple types */
template <> struct sum_of_size<std::tuple<>> {
  enum { value = 0 };
};

/** serialized size in bytes
 * @tparam target target type
 * @return serialize size in bytes
 */
template <typename target> constexpr size_t serialized_size() {
  return serializer<target>::size;
}

/** serialize
 * @tparam target target type
 * @tparam itor output iterator type
 * @return top of iterator pointing to the top of unused area
 */
template <typename target, typename itor>
itor serialize(itor begin, itor end, target const *obj) {
  return serializer<target>::serialize(begin, end, obj);
}

/** deserialize
 * @tparam target target type
 * @tparam itor input iterator type
 * @return top of iterator pointing to the top of unused area
 */
template <typename target, typename itor>
itor deserialize(itor begin, itor end, target *obj) {
  return deserializer<target>::deserialize(begin, end, obj);
}

/** deserialize
 * @tparam target target type
 * @tparam itor input iterator type
 * @return deserialized object
 */
template <typename target, typename itor>
target deserialize(itor begin, itor end) {
  return deserializer<target>::deserialize(begin, end);
}

} // namespace loleseri

/** type to serialize integer or floating point type
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::arithmetic> {

  /** type of the value to serialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** byte count of serialized size */
  enum { size = sizeof(target_type) };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

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
  using target_type = typename std::remove_cv<target_type_>::type;

  /** byte count of serialized size */
  enum { size = 1 };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

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
  using target_type = typename std::remove_cv<target_type_>::type;

  /** type to get list of items to serialize */
  using items = loleseri::items<target_type>;

  /** type of the list of items to serialize */
  using list_type = typename std::remove_cv<decltype(items::list())>::type;

  /** byte count of serialized size */
  enum { size = sum_of_size<list_type>::value };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

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
      constexpr size_t tc = std::tuple_size<list_type>::value;
      auto m = std::get<ix>(items::list());
      auto p = loleseri::serialize(begin, end, &(obj->*m));
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
    constexpr size_t tc = std::tuple_size<list_type>::value;
    return partial_serializer<0, (tc <= 0)>::serialize(begin, end, obj);
  }
};

/** type to serialize std::array
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::std_array> {
  /** type of the value to serialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** byte count of serialized size */
  enum {
    size = serializer<typename target_type::value_type>::size *
           std::tuple_size<target_type>::value
  };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
    auto p = begin;
    for (auto const &e : *obj) {
      p = loleseri::serialize(p, end, &e);
    }
    return p;
  }
};

/** type to serialize traditional array
 * @tparam target_type_ type of the value to serialize
 */
template <typename target_type_>
struct loleseri::serializer_impl<target_type_, loleseri::tcat::array> {
  /** type of the value to serialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** element type of traditional array "target_type" */
  using element_type = typename element_type_of_array<target_type>::type;

  /** byte count of serialized size */
  enum {
    element_count = size_of_array(target_type{}),
    size = serializer<element_type>::size * element_count
  };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t serialize(itor_t begin, itor_t end, target_type const *obj) {
    auto p = begin;
    for (auto const &e : *obj) {
      p = loleseri::serialize(p, end, &e);
    }
    return p;
  }
};

/** type to deserialize integer or floating point type
 * @tparam target_type_ type of the value to deserialize
 */
template <typename target_type_>
struct loleseri::deserializer_impl<target_type_, loleseri::tcat::arithmetic> {

  /** type of the value to deserialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** byte count of serialized size */
  enum { size = sizeof(target_type) };

  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator pointint to the top of the unused area
   */
  template <typename itor_t>
  static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
#if LOLESERI_LITTLE_ENDIAN
    auto p = reinterpret_cast<std::uint8_t *>(obj);
    std::copy(begin, begin + sizeof(target_type), p);
#else
#error "you should write something here."
#endif
    return begin + sizeof(target_type);
  }
  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @return deserialized object
   */
  template <typename itor_t>
  static target_type deserialize(itor_t begin, itor_t end) {
    target_type obj;
    deserialize(begin, end, &obj);
    return obj;
  }
};

/** type to deserialize boolean type
 * @tparam target_type_ type of the value to deserialize
 */
template <typename target_type_>
struct loleseri::deserializer_impl<target_type_, loleseri::tcat::boolean> {

  /** type of the value to deserialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** byte count of serialized size */
  enum { size = 1 };

  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator pointint to the top of the unused area
   */
  template <typename itor_t>
  static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
    *obj = !!*begin;
    return std::next(begin);
  }
  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @return deserialized object
   */
  template <typename itor_t>
  static target_type deserialize(itor_t begin, itor_t end) {
    return !!*begin;
  }
};

/** type to deserialize struct or class
 * @tparam target_type_ type of the value to deserialize
 */
template <typename target_type_>
struct loleseri::deserializer_impl<target_type_, loleseri::tcat::other> {
  /** target type */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** type to get list of items to serialize */
  using items = loleseri::items<target_type>;

  /** type of the list of items to serialize */
  using list_type = typename std::remove_cv<decltype(items::list())>::type;

  /** byte count of serialized size */
  enum { size = sum_of_size<list_type>::value };

  template <size_t ix, bool end_of_tuple> struct partial_deserializer;

  /** template to deserialize part of struct or class
   * @tparam ix skip first ix items
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @param[in] obj pointer
   * @return iterator which points to the begin of the unused area
   */
  template <size_t ix> struct partial_deserializer<ix, false> {
    /** deserialize part of struct or class
     * @tparam itor_t input iterator
     * @param[in] begin begin of input iterator
     * @param[in] end end of input iterator
     * @param[out] obj address to write the result of deserialize
     */
    template <typename itor_t>
    static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
      constexpr size_t tc = std::tuple_size<list_type>::value;
      auto m = std::get<ix>(items::list());
      using item_type = typename std::remove_reference<decltype(obj->*m)>::type;
      using deseri = loleseri::deserializer<item_type>;
      auto p = deseri::deserialize(begin, end, &(obj->*m));
      using partial = partial_deserializer<ix + 1, (tc <= ix + 1)>;
      return partial::deserialize(p, end, obj);
    }
  };

  /** template to deserialize part of struct or class
   * @tparam ix skip first ix items
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @param[in] obj pointer
   * @return iterator which points to the begin of the unused area
   */
  template <size_t ix> struct partial_deserializer<ix, true> {
    /** deserialize part of struct or class ( do nothing because ix is too big )
     * @tparam itor_t input iterator
     * @param[in] begin begin of input iterator
     * @param[in] end end of input iterator
     * @param[out] obj address to write the result of deserialize
     */
    template <typename itor_t>
    static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
      return begin;
    }
  };

  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator pointint to the top of the unused area
   */
  template <typename itor_t>
  static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
    constexpr size_t tc = std::tuple_size<list_type>::value;
    return partial_deserializer<0, (tc <= 0)>::deserialize(begin, end, obj);
  }
  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @return deserialized object
   */
  template <typename itor_t>
  static target_type deserialize(itor_t begin, itor_t end) {
    target_type obj;
    deserialize(begin, end, &obj);
    return obj;
  }
};

/** template to deserialize
 * @tparam target_type target type
 * @tparam typecat integer to specity category of target type
 */
template <typename target_type_>
struct loleseri::deserializer_impl<target_type_, loleseri::tcat::std_array> {
  /** type of the value to serialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  enum {
    /** byte count of serialized size */
    size = serializer<typename target_type::value_type>::size *
           std::tuple_size<target_type>::value
  };
  /** deserialize
   * @tparam itor_t input iterator
   * @param[in] begin begin of input iterator
   * @param[in] end end of input iterator
   * @param[obj] address to write the result of deserialize
   */
  template <typename itor_t>
  static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
    auto p = begin;
    using deseri = loleseri::deserializer<typename target_type::value_type>;
    for (auto &e : *obj) {
      p = deseri::deserialize(p, end, &e);
    }
    return p;
  }
  /** deserialize obj from input iterator
   * @tparam itor_t type of the input iterator
   * @param[in] begin top of the input iterator
   * @param[in] end end of the input iterator
   * @return deserialized object
   */
  template <typename itor_t>
  static target_type deserialize(itor_t begin, itor_t end) {
    target_type obj;
    deserialize(begin, end, &obj);
    return obj;
  }
};

/** template to deserialize
 * @tparam target_type target type
 * @tparam typecat integer to specity category of target type
 */
template <typename target_type_>
struct loleseri::deserializer_impl<target_type_, loleseri::tcat::array> {
  /** type of the value to serialize */
  using target_type = typename std::remove_cv<target_type_>::type;

  /** element type of traditional array "target_type" */
  using element_type = typename element_type_of_array<target_type>::type;

  enum {
    /** element count of traditional array "target_type" */
    element_count = size_of_array(target_type{}),

    /** byte count of serialized size */
    size = serializer<element_type>::size * element_count
  };

  /** type of array of the right size for serialization */
  using buffer = std::array<std::uint8_t, size>;

  /** serialize obj to output iterator
   * @tparam itor_t type of the output iterator
   * @param[in] begin top of the output iterator
   * @param[in] end end of the output iterator
   * @param[in] obj pointer to the object to serialize
   * @return iterator which points to the begin of the unused area
   */
  template <typename itor_t>
  static itor_t deserialize(itor_t begin, itor_t end, target_type *obj) {
    auto p = begin;
    using deseri = loleseri::deserializer<element_type>;
    for (auto &e : *obj) {
      p = deseri::deserialize(p, end, &e);
    }
    return p;
  }
};
