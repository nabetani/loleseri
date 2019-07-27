#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <tuple>

struct Foo {
  std::uint8_t hoge;
  float fuga;
};

namespace loleseri {
template <> struct items<Foo> {
  using list_type = std::tuple<std::uint8_t Foo::*, float Foo::*>;
  static inline list_type list() { return {&Foo::hoge, &Foo::fuga}; }
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
