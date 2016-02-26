#include "gtest/gtest.h"
#include "telegraph/pack.h"
#include "sky/protocol.h"
#include "util/methods.h"

class PackTest: public testing::Test {
 public:
  PackTest() { }

  ~PackTest() { }

  tg::Packet buffer;
};

/**
 * PacketWriters and PacketReaders operate correctly.
 */
TEST_F(PackTest, ReadWrite) {
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
  EXPECT_EQ(*reader.readValue<int>(), 2);
  EXPECT_EQ(*reader.readBit(), true);
  EXPECT_EQ(*reader.readChar(), 0xff);
  EXPECT_EQ((bool) reader.readChar(), false);
  // no more input to read, result is null
}

/**
 * Our basic pack rules work correctly.
 */
enum class MyEnum {
  ValueA, ValueB, ValueC, ValueD
};
TEST_F(PackTest, Rules) {
  const tg::Pack<float> floatPack = tg::BytePack<float>();
  const tg::Pack<std::string> stringPack = tg::StringPack();
  const tg::Pack<optional<std::string>> optPack =
      tg::OptionalPack<std::string>(stringPack);
  const tg::Pack<MyEnum> enumPack = tg::EnumPack<MyEnum>(2);
  const tg::Pack<std::list<float>> listPack = tg::ListPack<float>(floatPack);

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

  const std::list<float> testList = {1, 2, 3, 4};
  tg::packInto(listPack, testList, buffer);
  EXPECT_EQ(tg::unpack(listPack, buffer), testList);
  EXPECT_EQ(buffer.size, (sizeof(float) * 4) + sizeof(size_t));
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

TEST_F(PackTest, ClassPack) {
  MyStruct myStruct;
  myStruct.x = 5;
  tg::packInto(classPack, myStruct, buffer);
  MyStruct unpacked = *tg::unpack(classPack, buffer);
  EXPECT_EQ(unpacked.y, myStruct.y);
  EXPECT_EQ(unpacked.x, myStruct.x);
}

/**
 * Rules can fail when their input packets are malformed / too small.
 */
TEST_F(PackTest, FailTest) {
  // a packet that doesn't have a null terminating character can't be read as
  // a string
  tg::packInto(tg::boolPack, true, buffer);
  optional<std::string> unpackedStr = tg::unpack(tg::stringPack, buffer);
  EXPECT_EQ((bool) unpackedStr, false);

  // an int can't be read from a packet without 4 bytes
  tg::packInto(tg::BytePack<unsigned short int>(), (unsigned short) 0, buffer);
  optional<int> unpackedInt = tg::unpack(tg::intPack, buffer);
  EXPECT_EQ((bool) unpackedInt, false);
}

/**
 * Our MapPack rule works correctly.
 */
TEST_F(PackTest, MapPack) {
  const tg::Pack<std::map<int, MyStruct>> mapPack =
      tg::MapPack<int, MyStruct>(intPack, classPack);

  std::map<int, MyStruct> myMap;
  MyStruct myStruct1, myStruct2;
  myStruct1.x = 10;
  myStruct2.y = 5;
  myMap.emplace(0, myStruct1);
  myMap.emplace(2, myStruct2);
  tg::packInto(mapPack, myMap, buffer);

  std::map<int, MyStruct> unpacked = *tg::unpack(mapPack, buffer);
  EXPECT_EQ(unpacked[0].x, myStruct1.x);
  EXPECT_EQ(*unpacked[2].y, *myStruct2.y);
}

/**
 * Our ListlikePack rules work correctly.
 */
TEST_F(PackTest, ListlikePack) {
  const tg::Pack<std::vector<int>> vecPack = tg::VectorPack<int>(tg::intPack);

  std::vector<int> myVec = {1, 2, 5, 6};
  tg::packInto(vecPack, myVec, buffer);
  std::vector<int> unpacked = *tg::unpack(vecPack, buffer);
  EXPECT_EQ(myVec, unpacked);
  EXPECT_EQ(buffer.size, sizeof(size_t) + 4 * sizeof(int));
}

/**
 * Just a sanity test that protocol verb packing can work correctly.
 */
TEST_F(PackTest, ProtocolPack) {
  using namespace sky;

  tg::Pack<ClientPacket> clientPacketPack = ClientPacketPack();
  tg::Pack<ServerPacket> serverPacketPack = ServerPacketPack();

  ClientPacket packet = ClientPacket::ReqJoin("nickname");
  tg::packInto(clientPacketPack, packet, buffer);
  tg::unpackInto(clientPacketPack, buffer, packet);
  EXPECT_EQ(*packet.stringData, "nickname");
  EXPECT_EQ(packet.type, ClientPacket::Type::ReqJoin);

  ServerPacket serverPacket =
      ServerPacket::Message(ServerMessage::Broadcast("yeah"));
  tg::packInto(serverPacketPack, serverPacket, buffer);
  tg::unpackInto(serverPacketPack, buffer, serverPacket);
  EXPECT_EQ(serverPacket.message->contents, "yeah");
  EXPECT_EQ(serverPacket.type, ServerPacket::Type::Message);
}
