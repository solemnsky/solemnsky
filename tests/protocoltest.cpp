#include "engine/protocol.hpp"
#include "engine/sky/sky.hpp"
#include "util/methods.hpp"
#include "util/printer.hpp"
#include <gtest/gtest.h>
#include <cereal/archives/binary.hpp>

/**
 * Our multiplayer protocol verbs encode all network communication, and are
 * marshalled / unmarshalled correctly through cereal.
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
  optional<int> x;
  input(x);
  EXPECT_EQ(bool(x), true);
  EXPECT_EQ(x.get(), 6);

  // PlaneTuning: large, automatic type
  sky::PlaneTuning someTuning;
  someTuning.energy.laserGun = 0.5; // hax

  output(someTuning);

  someTuning.energy.laserGun = 1;
  input(someTuning);
  EXPECT_EQ(someTuning.energy.laserGun, 0.5);
}

/**
 * Protocol verbs serialize correctly.
 */
TEST_F(ProtocolTest, CerealProtocol) {
  {
    output(sky::ClientPacket::ReqJoin("hey"));
    sky::ClientPacket packet;
    input(packet);
    EXPECT_EQ(packet.verifyStructure(), true);
    EXPECT_EQ(packet.type, sky::ClientPacket::Type::ReqJoin);
    EXPECT_EQ(bool(packet.stringData), true);
    EXPECT_EQ(packet.stringData.get(), "hey");
  }

  {
    output(sky::ClientPacket::ReqSpawn());
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
  // Example: ClientPacket::ReqJoin needs a stringData.
  {
    sky::ClientPacket packet = sky::ClientPacket::ReqJoin("asdf");
    EXPECT_EQ(packet.verifyStructure(), true);
    packet.stringData.reset();
    EXPECT_EQ(verifyValue(packet), false);
  }

  // Example: ServerPacket::Chat needs a pid.
  {
    sky::ServerPacket packet =
        sky::ServerPacket::Broadcast("some broadcast");
    EXPECT_EQ(packet.verifyStructure(), true);
    packet.type = sky::ServerPacket::Type::Chat;
    EXPECT_EQ(packet.verifyStructure(), false);
    packet.type = (sky::ServerPacket::Type) 50;
    EXPECT_EQ(packet.verifyStructure(), false);
  }

}

