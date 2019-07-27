#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <type_traits>

TEST(Arithmetic, Uint8) {
  using seri = loleseri::serializer<std::uint8_t>;
  seri::buffer buffer;
  std::uint8_t value = 123;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 1>>::value,
                "buffer must be array<uint8_t,1>");
  ASSERT_EQ(1, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(value, buffer[0]);
}

TEST(Arithmetic, Uint16) {
  using seri = loleseri::serializer<std::uint16_t>;
  seri::buffer buffer;
  std::uint16_t value = 0xabcd;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 2>>::value,
                "buffer must be array<uint8_t,2>");
  ASSERT_EQ(2, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(0xcd, buffer[0]);
  ASSERT_EQ(0xab, buffer[1]);
}
