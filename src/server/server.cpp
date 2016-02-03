#include "server.h"
#include "pack/pack.h"
#include <iostream>

struct MyStruct {
  MyStruct() { }
  MyStruct(int x, optional<int> y) : x(x), y(y) {}

  int x = 0;
  optional<int> y{0};

  void dump() {
    std::cout << "x: ";
    std::cout << x;
    if (y) {
      std::cout << ", y: ";
      std::cout << *y;
    }
    std::cout << "\n";
  }
};

int main() {
  pk::Pack<optional<int>> optionalPack =
      pk::OptionalPack<int>(pk::BytePack<int>());
  pk::Pack<int> intPack = pk::BytePack<int>();

  pk::Pack<MyStruct> classPack = pk::ClassPack<MyStruct>(
      pk::MemberRule<MyStruct, int>(intPack, &MyStruct::x),
  pk::MemberRule<MyStruct, optional<int>>(optionalPack, &MyStruct::y)
  );

  pk::Packet packet;

  MyStruct x(24, 42);
  x.dump();
  packet = pack(classPack, x);
  packet.dumpBinary();
  x = unpack(classPack, packet);
  x.dump();
}
