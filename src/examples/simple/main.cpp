#include <array>
#include <cstdint>
#include <iostream>
#include <tuple>

#include <iomanip>
#include <ios>
#include <loleseri/loleseri.hpp>

/** 様々な算術型を持つ構造体 */
struct SimpleStruct {
  std::int8_t foo;
  std::int16_t bar;
  std::int32_t baz;
  std::int64_t qux;
  float quux;
  double corge;
  bool grault;
};

std::ostream &operator<<(std::ostream &o, SimpleStruct const &s) {
  return o                                 //
         << static_cast<int>(s.foo) << " " //
         << s.bar << " "                   //
         << s.baz << " "                   //
         << s.qux << " "                   //
         << s.quux << " "                  //
         << s.corge << " "                 //
         << std::boolalpha << s.grault;
}

/** SimpleStructのメンバリスト */
auto const simpleStructMembers =
    std::make_tuple(&SimpleStruct::foo, &SimpleStruct::bar, &SimpleStruct::baz,
                    &SimpleStruct::qux, &SimpleStruct::quux,
                    &SimpleStruct::corge, &SimpleStruct::grault);

/** シリアライザから見えるようにする */
template <> struct loleseri::items<SimpleStruct> {
  static decltype(simpleStructMembers) list() { return simpleStructMembers; }
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
  constexpr SimpleStruct hoge{1, 2, 3, 4, 5, 6, true};

  // 必要なサイズをコンパイル時に計算する
  constexpr size_t size = loleseri::serialized_size<SimpleStruct>();

  // バッファを確保。vector でも deque でも生メモリでもよい。
  std::array<std::uint8_t, size> buffer;

  // シリアライズ
  loleseri::serialize(buffer.begin(), buffer.end(), &hoge);

  // シリアライズ結果をダンプ
  dump(buffer);

  // 復元
  auto fuga =
      loleseri::deserialize<SimpleStruct>(buffer.cbegin(), buffer.cend());

  std::cout << "hoge: " << hoge << "\n"
            << "fuga: " << fuga << std::endl;
}
