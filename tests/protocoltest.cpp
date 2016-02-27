#include <gtest/gtest.h>
#include "sky/protocol.h"

/**
 * Our multiplayer protocol structs (ServerPacket, ClientPacket) and
 * associating packing rules.
 */
class ProtocolTest: public testing::Test {
 public:
  ProtocolTest() { }
  tg::Packet buffer;
};

/**
 * Sanity check for clientPacketPack.
 */
TEST_F(ProtocolTest, ClientPacket) {

}

/**
 * Sanity check for serverPacketPack.
 */
TEST_F(ProtocolTest, ServerPacket) {

}
