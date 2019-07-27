#include <gtest/gtest.h>
#include <loleseri/loleseri.hpp>
#include <type_traits>

TEST(Arithmetic, Uint8) {
  using seri = loleseri::serializer<std::uint8_t>;
  seri::buffer buffer;
  std::uint8_t const value = 123;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 1>>::value,
                "buffer must be array<uint8_t,1>");
  ASSERT_EQ(1, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(value, buffer[0]);

  using deseri = loleseri::deserializer<std::uint8_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::uint8_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );
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

  using deseri = loleseri::deserializer<std::uint16_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::uint16_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );
}

TEST(Arithmetic, Uint32) {
  using seri = loleseri::serializer<std::uint32_t>;
  seri::buffer buffer;
  std::uint32_t value = 0x1234abcd;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 4>>::value,
                "buffer must be array<uint8_t,4>");
  ASSERT_EQ(4, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(0xcd, buffer[0]);
  ASSERT_EQ(0xab, buffer[1]);
  ASSERT_EQ(0x34, buffer[2]);
  ASSERT_EQ(0x12, buffer[3]);

  using deseri = loleseri::deserializer<std::uint32_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::uint32_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );

}

TEST(Arithmetic, Uint64) {
  using seri = loleseri::serializer<std::uint64_t>;
  seri::buffer buffer;
  std::uint64_t value = 0xb13260d357679c5c; // 乱数で作った
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 8>>::value,
                "buffer must be array<uint8_t,8>");
  ASSERT_EQ(8, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(0x5c, buffer[0]);
  ASSERT_EQ(0x9c, buffer[1]);
  ASSERT_EQ(0x67, buffer[2]);
  ASSERT_EQ(0x57, buffer[3]);
  ASSERT_EQ(0xd3, buffer[4]);
  ASSERT_EQ(0x60, buffer[5]);
  ASSERT_EQ(0x32, buffer[6]);
  ASSERT_EQ(0xb1, buffer[7]);

  using deseri = loleseri::deserializer<std::uint64_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::uint64_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );

}

TEST(Arithmetic, Int8) {
  using seri = loleseri::serializer<std::int8_t>;
  seri::buffer buffer;
  std::int8_t value = 123;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 1>>::value,
                "buffer must be array<uint8_t,1>");
  ASSERT_EQ(1, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(value, buffer[0]);


  using deseri = loleseri::deserializer<std::int8_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::int8_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );
}

TEST(Arithmetic, Int16) {
  using seri = loleseri::serializer<std::int16_t>;
  seri::buffer buffer;
  std::int16_t value = 0x1234;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 2>>::value,
                "buffer must be array<uint8_t,2>");
  ASSERT_EQ(2, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(0x34, buffer[0]);
  ASSERT_EQ(0x12, buffer[1]);

  using deseri = loleseri::deserializer<std::int16_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::int16_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );


}

TEST(Arithmetic, Int32) {
  using seri = loleseri::serializer<std::int32_t>;
  seri::buffer buffer;
  std::int32_t value = 0x1234abcd;
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 4>>::value,
                "buffer must be array<uint8_t,4>");
  ASSERT_EQ(4, buffer.size());
  ASSERT_EQ(buffer.end(), last);
  ASSERT_EQ(0xcd, buffer[0]);
  ASSERT_EQ(0xab, buffer[1]);
  ASSERT_EQ(0x34, buffer[2]);
  ASSERT_EQ(0x12, buffer[3]);

  using deseri = loleseri::deserializer<std::int32_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::int32_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );
}

TEST(Arithmetic, Int64) {
  using seri = loleseri::serializer<std::int64_t>;
  seri::buffer buffer;
  std::int64_t value = 0x1677a4815561cdf3; // 乱数で作った
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 8>>::value,
                "buffer must be array<uint8_t,8>");
  ASSERT_EQ(8, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0xf3, buffer[0]);
  ASSERT_EQ(0xcd, buffer[1]);
  ASSERT_EQ(0x61, buffer[2]);
  ASSERT_EQ(0x55, buffer[3]);
  ASSERT_EQ(0x81, buffer[4]);
  ASSERT_EQ(0xa4, buffer[5]);
  ASSERT_EQ(0x77, buffer[6]);
  ASSERT_EQ(0x16, buffer[7]);

  using deseri = loleseri::deserializer<std::int64_t>;
  auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
  std::int64_t v1;
  deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

  ASSERT_EQ( value, v0 );
  ASSERT_EQ( value, v1 );
}

TEST(Arithmetic, Boolean) {
  auto impl = [&](bool value, std::uint8_t serival) {
    using seri = loleseri::serializer<bool>;
    seri::buffer buffer;
    ASSERT_EQ(1, buffer.size());
    static_assert(
        std::is_same<seri::buffer, std::array<std::uint8_t, 1>>::value,
        "buffer must be array<uint8_t,1>");
    auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
    ASSERT_EQ(buffer.end(), last);

    ASSERT_EQ(serival, buffer[0]);

    using deseri = loleseri::deserializer<bool>;
    auto v0 = deseri::deserialize( buffer.cbegin(), buffer.cend() );
    bool v1;
    deseri::deserialize( buffer.begin(), buffer.end(), &v1 );

    ASSERT_EQ( value, v0 );
    ASSERT_EQ( value, v1 );
  };
  impl(true, 1);
  impl(false, 0);
}

TEST(Arithmetic, Float) {
  using seri = loleseri::serializer<float>;
  seri::buffer buffer;
  float value = 123.456f; // リトルエンディアンで "79,e9,f6,42"
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 4>>::value,
                "buffer must be array<uint8_t,4>");
  ASSERT_EQ(4, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0x79, buffer[0]);
  ASSERT_EQ(0xe9, buffer[1]);
  ASSERT_EQ(0xf6, buffer[2]);
  ASSERT_EQ(0x42, buffer[3]);
}

TEST(Arithmetic, Double) {
  using seri = loleseri::serializer<double>;
  seri::buffer buffer;
  double value =
      123456.654321; // リトルエンディアンで "01,4c,19,78,0a,24,fe,40"
  auto last = seri::serialize(buffer.begin(), buffer.end(), &value);
  static_assert(std::is_same<seri::buffer, std::array<std::uint8_t, 8>>::value,
                "buffer must be array<uint8_t,8>");
  ASSERT_EQ(8, buffer.size());
  ASSERT_EQ(buffer.end(), last);

  ASSERT_EQ(0x01, buffer[0]);
  ASSERT_EQ(0x4c, buffer[1]);
  ASSERT_EQ(0x19, buffer[2]);
  ASSERT_EQ(0x78, buffer[3]);
  ASSERT_EQ(0x0a, buffer[4]);
  ASSERT_EQ(0x24, buffer[5]);
  ASSERT_EQ(0xfe, buffer[6]);
  ASSERT_EQ(0x40, buffer[7]);
}
