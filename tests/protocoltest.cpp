#include <gtest/gtest.h>
#include "sky/protocol.h"
#include "sky/sky.h"
#include "util/methods.h"
#include <cereal/archives/binary.hpp>

/**
 * Our multiplayer protocol structs (ServerPacket, ClientPacket) and
 * associating packing rules.
 */
class ProtocolTest: public testing::Test {
 public:
  std::stringstream stream;
  cereal::BinaryOutputArchive output;
  cereal::BinaryInputArchive input;

  ProtocolTest() :
      output(stream), input(stream) { }
};

/**
 * Cereal works like we expect it to.
 */

TEST_F(ProtocolTest, Cereal) {
  // optionals: small, custom type
  output(optional<int>(6));
  appLog(stream.str());
  optional<int> x;
  input(x);
  EXPECT_EQ(bool(x), true);
  EXPECT_EQ(x.get(), 6);

  // PlaneTuning: large, automatic type
  sky::PlaneTuning someTuning;
  someTuning.energy.laserGun = 0.5; // hax dude

  output(someTuning);
  appLog(stream.str());

  someTuning.energy.laserGun = 1;
  input(someTuning);
  EXPECT_EQ(someTuning.energy.laserGun, 0.5);
}

/**
 * Protocol elements serialize correctly.
 */
TEST_F(ProtocolTest, CerealProtocol) {
  {
    output(sky::ClientPacket::ReqJoin("hey"));
    appLog(stream.str());
    sky::ClientPacket packet;
    input(packet);
    EXPECT_EQ(packet.verifyStructure(), true);
    EXPECT_EQ(packet.type, sky::ClientPacket::Type::ReqJoin);
    EXPECT_EQ(bool(packet.stringData), true);
    EXPECT_EQ(packet.stringData.get(), "hey");
  }

  {
    output(sky::ClientPacket::ReqSpawn());
    appLog(stream.str());
    sky::ClientPacket packet;
    input(packet);
    EXPECT_EQ(packet.verifyStructure(), true);
    EXPECT_EQ(packet.type, sky::ClientPacket::Type::ReqSpawn);
  }
}

/**
 * Invariant violations can be caught in protocol packets.
 */
TEST_F(ProtocolTest, Invariant) {
  sky::ClientPacket packet = sky::ClientPacket::ReqJoin("asdf");
  EXPECT_EQ(packet.verifyStructure(), true);
  packet.stringData.reset();
  EXPECT_EQ(verifyValue(packet), false);

  sky::ServerPacket sPacket =
      sky::ServerPacket::Message(
          sky::ServerMessage::Broadcast("some broadcast"));
  EXPECT_EQ(sPacket.verifyStructure(), true);
  sPacket.stringData->type = sky::ServerMessage::Type::Chat;
  EXPECT_EQ(sPacket.verifyStructure(), false);
}
