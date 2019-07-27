#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <tuple>

namespace {
struct Foo {
  std::uint8_t hoge;
  float fuga;
};

bool operator==(Foo const &a, Foo const &b) {
  return a.hoge == b.hoge && a.fuga == b.fuga;
}

bool operator!=(Foo const &a, Foo const &b) { return !(a == b); }

struct Bar {
  std::uint16_t orange;
  Foo banana;
};

bool operator==(Bar const &a, Bar const &b) {
  return a.orange == b.orange && a.banana == b.banana;
}

bool operator!=(Bar const &a, Bar const &b) { return !(a == b); }

struct Baz {
  Foo grape[2];
  std::uint8_t kiwi[5];
  Bar melon[3];
  static Baz create() {
    Baz r;
    int x = 0;
    for (auto &e : r.grape) {
      e.fuga = ++x;
      e.hoge = ++x;
    }
    for (auto &e : r.kiwi) {
      e = ++x;
    }
    for (auto &e : r.melon) {
      e.banana.fuga = ++x;
      e.banana.hoge = ++x;
      e.orange = ++x;
    }
    return r;
  }
};

bool operator==(Baz const &a, Baz const &b) {
  for (int i = 0; i < 2; ++i) {
    if (a.grape[i] != b.grape[i]) {
      return false;
    }
  }
  for (int i = 0; i < 5; ++i) {
    if (a.kiwi[i] != b.kiwi[i]) {
      return false;
    }
  }
  for (int i = 0; i < 3; ++i) {
    if (a.melon[i] != b.melon[i]) {
      return false;
    }
  }
  return true;
}

const auto bazMembers = std::make_tuple(&Baz::grape, &Baz::kiwi, &Baz::melon);

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

template <> struct items<Baz> {
  static inline decltype(bazMembers) list() { return bazMembers; }
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

  using deseri = loleseri::deserializer<Foo>;
  auto v0 = deseri::deserialize(buffer.cbegin(), buffer.cend());
  Foo v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);

  ASSERT_EQ(value, v0);
  ASSERT_EQ(value, v1);
}

TEST(Struct, StructInStruct) {
  using seri = loleseri::serializer<Bar>;
  seri::buffer buffer;
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

  using deseri = loleseri::deserializer<Bar>;
  auto v0 = deseri::deserialize(buffer.cbegin(), buffer.cend());
  Bar v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);

  ASSERT_EQ(value, v0);
  ASSERT_EQ(value, v1);
}

TEST(Struct, ArrayInStruct) {
  using seri = loleseri::serializer<Baz>;
  seri::buffer buffer;
  Baz value = Baz::create();
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  ASSERT_EQ(buffer.end(), last);
  using deseri = loleseri::deserializer<Baz>;
  auto v0 = deseri::deserialize(buffer.cbegin(), buffer.cend());
  Baz v1;
  deseri::deserialize(buffer.begin(), buffer.end(), &v1);
  ASSERT_EQ(value, v0);
  ASSERT_EQ(value, v1);
}
