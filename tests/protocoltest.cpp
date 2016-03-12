#include <gtest/gtest.h>
#include "sky/protocol.h"
#include "util/methods.h"
#include <cereal/archives/binary.hpp>

/**
 * Our multiplayer protocol structs (ServerPacket, ClientPacket) and
 * associating packing rules.
 */
class ProtocolTest: public testing::Test {
 public:
  ProtocolTest() { }
};

/**
 * Cereal works like we expect it to.
 */
TEST_F(ProtocolTest, Cereal) {
  std::stringstream stream;
  sky::PlaneTuning someTuning;
  someTuning.energy.laserGun = 0.5; // hax dude

  cereal::BinaryOutputArchive output(stream);
  cereal::BinaryInputArchive input(stream);

  output(someTuning);
  std::cout << stream.str();
  std::endl(std::cout);

  someTuning.energy.laserGun = 1;
  input(someTuning);
  EXPECT_EQ(someTuning.energy.laserGun, 0.5);
}

/**
 * Invariant violations can be caught in protocol packets.
 */
TEST_F(ProtocolTest, InvariantTest) {
  sky::ClientPacket packet = sky::ClientPacket::ReqJoin("asdf");
  EXPECT_TRUE(packet.verifyStructure());
  packet.stringData.reset();
  EXPECT_FALSE(verifyValue(packet));

  sky::ServerPacket sPacket =
      sky::ServerPacket::Message(
          sky::ServerMessage::Broadcast("some broadcast"));
  EXPECT_TRUE(sPacket.verifyStructure());
  sPacket.message->type = sky::ServerMessage::Type::Chat;
//  EXPECT_FALSE(sPacket.verifyStructure()); // TODO: make this work
}
