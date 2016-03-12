#include <gtest/gtest.h>
#include "sky/protocol.h"
#include "util/methods.h"

/**
 * Our multiplayer protocol structs (ServerPacket, ClientPacket) and
 * associating packing rules.
 */
class ProtocolTest: public testing::Test {
 public:
  ProtocolTest() { }
};

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
