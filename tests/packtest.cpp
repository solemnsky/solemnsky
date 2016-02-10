#include "gtest/gtest.h"
#include "telegraph/pack.h"

class PackFixture: public testing::Test {
 public:
  PackFixture() { }

  ~PackFixture() { }

  tg::Packet buffer;
};

/**
 * PacketWriters and PacketReaders operate correctly.
 */
TEST_F(PackFixture, ReadWrite) {
  const tg::Pack<bool> boolPack = tg::BoolPack();
  const tg::Pack<int> intPack = tg::BytePack<int>();

  tg::PacketWriter writer{&buffer};
  tg::PacketReader reader{&buffer};

  writer.writeValue<int>(2);
  writer.writeBit(true);
  writer.writeChar(0xFF);
  EXPECT_EQ(buffer.dumpBinary(),
            "00000010 00000000 00000000 00000000 11111111 00000001 ");
  //        | int                              | |char ||        |
  //                                                    bit      char
  EXPECT_EQ(reader.readValue<int>(), 2);
  EXPECT_EQ(reader.readBit(), true);
  EXPECT_EQ(reader.readChar(), 0xff);
//  EXPECT_DEATH(reader.readChar(), ".*");
}

/**
 * Our basic pack rules work correctly.
 */
enum class MyEnum {
  ValueA, ValueB, ValueC, ValueD
};
TEST_F(PackFixture, Rules) {
  const tg::Pack<float> floatPack = tg::BytePack<float>();
  const tg::Pack<std::string> stringPack = tg::StringPack();
  const tg::Pack<optional<std::string>> optPack =
      tg::OptionalPack<std::string>(stringPack);
  const tg::Pack<MyEnum> enumPack = tg::EnumPack<MyEnum>(2);

  const float testFloat = 5.23f;
  tg::packInto(floatPack, testFloat, buffer);
  EXPECT_EQ(tg::unpack(floatPack, buffer), testFloat);
  EXPECT_EQ(buffer.size, sizeof(float));

  const std::string testString = "hello world";
  tg::packInto(stringPack, testString, buffer);
  EXPECT_EQ(tg::unpack(stringPack, buffer), testString);
  EXPECT_EQ(buffer.size, testString.length() + 1); // 1 byte for length

  const optional<std::string> testValue = testString;
  tg::packInto(optPack, testValue, buffer);
  EXPECT_EQ(tg::unpack(optPack, buffer), testValue);
  EXPECT_EQ(buffer.size, testString.length() + 2);
  // the 1 bit for optional state, bumps us into another byte

  const MyEnum testEnum = MyEnum::ValueC;
  tg::packInto(enumPack, testEnum, buffer);
  EXPECT_EQ(tg::unpack(enumPack, buffer), testEnum);
  EXPECT_EQ(buffer.size, 1);
}

/**
 * Our ClassPack rule works correctly.
 */
struct MyStruct {
  MyStruct() = default;
  int x = 0;
  optional<int> y = 0;
};

const tg::Pack<int> intPack = tg::BytePack<int>();
const tg::Pack<optional<int>> optPack = tg::OptionalPack<int>(intPack);
const tg::Pack<MyStruct> classPack =
    tg::ClassPack<MyStruct>(
        tg::MemberRule<MyStruct, int>(intPack, &MyStruct::x),
        tg::MemberRule<MyStruct, optional<int>>(optPack, &MyStruct::y)
    );

TEST_F(PackFixture, ClassPack) {
  MyStruct myStruct;
  myStruct.x = 5;
  tg::packInto(classPack, myStruct, buffer);
  MyStruct unpacked = tg::unpack(classPack, buffer);
  EXPECT_EQ(unpacked.y, myStruct.y);
  EXPECT_EQ(unpacked.x, myStruct.x);
}

/**
 * Our MapPack rule works correctly.
 */
TEST_F(PackFixture, MapPack) {
  const tg::Pack<std::map<int, MyStruct>> mapPack =
      tg::MapPack<int, MyStruct>(intPack, classPack);

  std::map<int, MyStruct> myMap;
  MyStruct myStruct1, myStruct2;
  myStruct1.x = 10;
  myStruct2.y = 5;
  myMap.emplace(0, myStruct1);
  myMap.emplace(2, myStruct2);
  tg::packInto(mapPack, myMap, buffer);

  std::map<int, MyStruct> unpacked = tg::unpack(mapPack, buffer);
  EXPECT_EQ(unpacked[0].x, myStruct1.x);
  EXPECT_EQ(*unpacked[2].y, *myStruct2.y);
}
