#include <array>
#include <cstdint>
#include <iostream>
#include <tuple>

#include <loleseri/loleseri.hpp>

struct Foo {
  std::uint8_t hoge_;
  std::int32_t fuga_;
  float piyo_;
};

namespace loleseri {
template <> struct items<Foo> {
  using list_type = std::tuple<
    std::uint8_t Foo::*,
    std::int32_t Foo::*,
    float Foo::*>;
  static inline list_type
  list(){
      return {&Foo::hoge_, &Foo::fuga_, &Foo::piyo_};
  }
};
} // namespace loleseri

int main()
{
  std::array<std::uint8_t, loleseri::size<Foo>()> buffer;
  Foo foo;
  loleseri::serialize( buffer.begin(), buffer.end(), &foo );
}
