#include <gtest/gtest.h>
#include <sky/protocol.h>
#include "sky/arena.h"
#include "util/methods.h"

class ArenaTest: public testing::Test {
 public:
  tg::Packet buffer;
};

TEST_F(ArenaTest, ConnectionTest) {
  sky::Arena arena;
  sky::Player &player1 = arena.connectPlayer();
  EXPECT_EQ(player1.pid, 1);
  sky::Player &player2 = arena.connectPlayer();
  EXPECT_EQ(player2.pid, 2);

  player1.nickname = "asdf";
  player2.nickname = "fdsa";
  EXPECT_EQ(arena.getRecord(1)->nickname, "asdf");
  arena.disconnectPlayer(player1);
  EXPECT_EQ(arena.getRecord(1), nullptr);
  EXPECT_EQ(arena.getRecord(2)->nickname, "fdsa");
  arena.disconnectPlayer(player2);
  EXPECT_EQ(arena.getRecord(2), nullptr);
}

TEST_F(ArenaTest, InitializationTest) {
  sky::Arena arena;
  arena.motd = "test server";
  sky::Player &player1 = arena.connectPlayer();
  player1.nickname = "somebody";

  sky::ArenaInitializer initializer = arena.captureInitializer();
  sky::prot::ServerPacket ack = sky::prot::ServerAckJoin(1, initializer);
  tg::packInto(sky::prot::serverPacketPack, ack, buffer);

  sky::prot::ServerPacket receivedAck =
      tg::unpack(sky::prot::serverPacketPack, buffer);
  sky::Arena remoteArena = *receivedAck.arenaInitializer;

  EXPECT_EQ(remoteArena.getRecord(*receivedAck.pid)->nickname, "somebody");
  EXPECT_EQ(remoteArena.motd, "test server");
}

TEST_F(ArenaTest, DeltaTest) {
  sky::Arena arena;
  arena.motd = "test server";

  sky::ArenaDelta delta;
  delta.motdDelta = "secret server";
  tg::packInto(sky::arenaDeltaPack, delta, buffer);
  appLog(buffer.dump());
  arena.applyDelta(tg::unpack(sky::arenaDeltaPack, buffer));
  EXPECT_EQ(arena.motd, "secret server");

  delta = {};
  sky::Player player(1);
  player.nickname = "somebody";
  delta.playerJoin = player;
  tg::packInto(sky::arenaDeltaPack, delta, buffer);
  appLog(buffer.dump());
  arena.applyDelta(tg::unpack(sky::arenaDeltaPack, buffer));
  EXPECT_NE(arena.getRecord(1), nullptr);
  EXPECT_EQ(arena.getRecord(1)->nickname, "somebody");

  delta = {};
  delta.playerDelta = std::pair<sky::PID, sky::PlayerDelta>(
      1, sky::PlayerDelta(std::string("somebody else")));
  tg::packInto(sky::arenaDeltaPack, delta, buffer);
  appLog(buffer.dump());
  arena.applyDelta(tg::unpack(sky::arenaDeltaPack, buffer));
  EXPECT_EQ(arena.getRecord(1)->nickname, "somebody else");

  delta = {};
  delta.playerQuit = 1;
  tg::packInto(sky::arenaDeltaPack, delta, buffer);
  appLog(buffer.dump());
  arena.applyDelta(tg::unpack(sky::arenaDeltaPack, buffer));
  EXPECT_EQ(arena.getRecord(1), nullptr);
}

