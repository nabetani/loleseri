#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <tuple>

namespace {
struct Foo {
  double hoge;
  std::int32_t fuga;
};

bool operator==(Foo const &a, Foo const &b) {
  return a.hoge == b.hoge && a.fuga == b.fuga;
}

bool operator!=(Foo const &a, Foo const &b) { return !(a == b); }

struct Bar {
  Foo banana;
  std::int8_t orange;
};

bool operator==(Bar const &a, Bar const &b) {
  return a.orange == b.orange && a.banana == b.banana;
}

bool operator!=(Bar const &a, Bar const &b) { return !(a == b); }

} // namespace

namespace loleseri {

template <> struct items<Foo> {
  using list_type = std::tuple<double Foo::*, std::int32_t Foo::*>;
  static inline list_type list() { return {&Foo::hoge, &Foo::fuga}; }
};

template <> struct items<Bar> {
  using list_type = std::tuple<Foo Bar::*, std::int8_t Bar::*>;
  static inline list_type list() { return {&Bar::banana, &Bar::orange}; }
};
} // namespace loleseri

TEST(StdArray, Bar) {
  using array_t = std::array<Bar, 11>;
  using seri = loleseri::serializer<array_t>;
  static_assert(
      std::is_same<seri::buffer, std::array<std::uint8_t, 13 * 11>>::value,
      "buffer must be array<uint8_t,13*11>");
  array_t value;
  seri::buffer buffer;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  ASSERT_EQ(13 * 11, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  using deseri = loleseri::deserializer<array_t>;
  auto v0 = deseri::deserialize(buffer.cbegin(), buffer.cend());
  array_t v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);

  ASSERT_EQ(value, v0);
  ASSERT_EQ(value, v1);
}

TEST(StdArray, Uint8) {
  using array_t = std::array<std::uint8_t, 7>;
  using seri = loleseri::serializer<array_t>;
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 7>>::value,
                "buffer must be array<uint8_t,7>");
  array_t value = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
  seri::buffer buffer;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  ASSERT_EQ(7, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0x11, buffer[0]);
  ASSERT_EQ(0x22, buffer[1]);
  ASSERT_EQ(0x33, buffer[2]);
  ASSERT_EQ(0x44, buffer[3]);
  ASSERT_EQ(0x55, buffer[4]);
  ASSERT_EQ(0x66, buffer[5]);
  ASSERT_EQ(0x77, buffer[6]);

  using deseri = loleseri::deserializer<array_t>;
  auto v0 = deseri::deserialize(buffer.cbegin(), buffer.cend());
  array_t v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);

  ASSERT_EQ(value, v0);
  ASSERT_EQ(value, v1);
}

TEST(Array, Uint16) {
  using array_t = std::uint16_t[3];
  using seri = loleseri::serializer<array_t>;
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 6>>::value,
                "buffer must be array<uint8_t,6>");
  array_t value = {0xa1b2, 0xc3d4, 0xe5f6};
  seri::buffer buffer;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  ASSERT_EQ(6, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0xb2, buffer[0]);
  ASSERT_EQ(0xa1, buffer[1]);
  ASSERT_EQ(0xd4, buffer[2]);
  ASSERT_EQ(0xc3, buffer[3]);
  ASSERT_EQ(0xf6, buffer[4]);
  ASSERT_EQ(0xe5, buffer[5]);

  using deseri = loleseri::deserializer<array_t>;

  // traditional array cannot be a return value.
  // auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );

  array_t v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);

  for (int i = 0; i < 3; ++i) {
    ASSERT_EQ(value[i], v1[i]);
  }
}
