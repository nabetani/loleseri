#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <tuple>

namespace {
struct Foo {
  std::uint8_t hoge;
  float fuga;
};

struct Bar {
  std::uint16_t orange;
  Foo banana;
};
} // namespace

namespace loleseri {

template <> struct items<Foo> {
  using list_type = std::tuple<std::uint8_t Foo::*, float Foo::*>;
  static inline list_type list() { return {&Foo::hoge, &Foo::fuga}; }
};

template <> struct items<Bar> {
  using list_type = std::tuple<std::uint16_t Bar::*, Foo Bar::*>;
  static inline list_type list() { return {&Bar::orange, &Bar::banana}; }
};

} // namespace loleseri

TEST(Struct, Simple) {
  using seri = loleseri::serializer<Foo>;
  seri::buffer buffer;
  Foo value = {123, 987.654f}; // 987.654f は、リトルエンディアンで"db,e9,76,44"
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 5>>::value,
                "buffer must be array<uint8_t,5>");
  ASSERT_EQ(5, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0x7b, buffer[0]);
  ASSERT_EQ(0xdb, buffer[1]);
  ASSERT_EQ(0xe9, buffer[2]);
  ASSERT_EQ(0x76, buffer[3]);
  ASSERT_EQ(0x44, buffer[4]);
}

TEST(Struct, StructInStruct) {
  using seri = loleseri::serializer<Bar>;
  seri::buffer buffer;
  std::cout << "buffer.size() = " << buffer.size() << "\n";
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 7>>::value,
                "buffer must be array<uint8_t,7>");
  Bar value = {0x9876, {11, 2233.4455f}};
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  ASSERT_EQ(7, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0x76, buffer[0]);
  ASSERT_EQ(0x98, buffer[1]);
  ASSERT_EQ(0x0b, buffer[2]);
  ASSERT_EQ(0x21, buffer[3]);
  ASSERT_EQ(0x97, buffer[4]);
  ASSERT_EQ(0x0b, buffer[5]);
  ASSERT_EQ(0x45, buffer[6]);
}
