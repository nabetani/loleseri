#include <array>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <loleseri/loleseri.hpp>
#include <tuple>

template <typename element> struct point { element x, y, z; };

template <typename element>
std::ostream &operator<<(std::ostream &o, point<element> const &pt) {
  return o << "[" << pt.x << ", " << pt.y << ", " << pt.z << "]";
}

struct foo {
  point<float> bar;                // struct with template
  std::array<point<int>, 3> baz;   // std::array of struct with template
  point<std::uint16_t> qux[2];     // traditional array of struct with template
  point<point<std::int16_t>> quux; // struct with struct with template

  static foo create(int i) {
    int b = i * 100;
    foo v;
    v.bar.x = (v.bar.y = (v.bar.z = (b += 10)) + 1) + 1;
    for (size_t i = 0; i < v.baz.size(); ++i) {
      auto &t = v.baz[i];
      t.z = (t.y = (t.x = (b += 10)) + 1) + 1;
    }
    for (size_t i = 0; i < 2; ++i) {
      auto &t = v.qux[i];
      t.z = (t.y = (t.x = (b += 10)) + 1) + 1;
    }
    for (auto p : {&v.quux.x, &v.quux.y, &v.quux.z}) {
      auto & t = *p;
      t.z = (t.y = (t.x = (b += 10)) + 1) + 1;
    }
    return v;
  }
};

std::ostream &operator<<(std::ostream &o, foo const &v) {
  o << std::dec;
  o << "bar: " << v.bar;
  o << " / baz:";
  for (auto const &e : v.baz) {
    o << " " << e;
  }
  o << " / qux:";
  for (auto const &e : v.qux) {
    o << " " << e;
  }
  o << " / quux: " << v.quux;
  return o;
}

/** member list of point */
template <typename element> struct loleseri::items<point<element>> {
  using pt = point<element>;
  static decltype(std::make_tuple(&pt::x, &pt::y, &pt::z)) list() {
    return std::make_tuple(&pt::x, &pt::y, &pt::z);
  }
};

/** member list of foo */
auto const foo_members =
    std::make_tuple(&foo::bar, &foo::baz, &foo::qux, &foo::quux);

template <> struct loleseri::items<foo> {
  static decltype(foo_members) list() { return foo_members; }
};

template <typename container> //
void dump(container const &c) {
  using namespace std;
  for (auto const &e : c) {
    cout << setw(2) << setfill('0') << hex << static_cast<int>(e) << " ";
  }
  cout << endl;
}

int main() {
  std::array<foo, 3> val{{foo::create(1), foo::create(2), foo::create(3)}};

  std::cout << "val:" << std::endl;
  for (auto const &e : val) {
    std::cout << e << std::endl;
  }

  // 必要なサイズをコンパイル時に計算する
  constexpr size_t size = loleseri::serialized_size<decltype(val)>();

  // バッファを確保。vector でも deque でも生メモリでもよい。
  std::array<std::uint8_t, size> buffer;

  // シリアライズ
  loleseri::serialize(buffer.begin(), buffer.end(), &val);

  // シリアライズ結果をダンプ
  dump(buffer);

  // 復元
  auto restored =
      loleseri::deserialize<decltype(val)>(buffer.cbegin(), buffer.cend());

  std::cout << "restored:" << std::endl;
  for (auto const &e : restored) {
    std::cout << e << std::endl;
  }
}
