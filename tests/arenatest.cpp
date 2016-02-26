#include <gtest/gtest.h>
#include <sky/protocol.h>
#include "sky/arena.h"
#include "util/methods.h"

class ArenaTest: public testing::Test {
 public:
  tg::Packet buffer;

  const tg::Pack<sky::ServerPacket> serverPacketPack = sky::ServerPacketPack();
  const tg::Pack<sky::ArenaDelta> arenaDeltaPack = sky::ArenaDeltaPack();
};

TEST_F(ArenaTest, ConnectionTest) {
  sky::Arena arena;
  sky::Player &player1 = arena.connectPlayer();
  EXPECT_EQ(player1.pid, 1);
  sky::Player &player2 = arena.connectPlayer();
  EXPECT_EQ(player2.pid, 2);

  player1.nickname = "asdf";
  player2.nickname = "fdsa";
  EXPECT_EQ(arena.getPlayer(1)->nickname, "asdf");
  arena.disconnectPlayer(player1);
  EXPECT_EQ(arena.getPlayer(1), nullptr);
  EXPECT_EQ(arena.getPlayer(2)->nickname, "fdsa");
  arena.disconnectPlayer(player2);
  EXPECT_EQ(arena.getPlayer(2), nullptr);
}

TEST_F(ArenaTest, InitializationTest) {
  sky::Arena arena;
  arena.motd = "test server";
  sky::Player &player1 = arena.connectPlayer();
  player1.nickname = "somebody";

  sky::ArenaInitializer initializer = arena.captureInitializer();
  sky::ServerPacket ack = sky::ServerPacket::AckJoin(1, initializer);
  tg::packInto(serverPacketPack, ack, buffer);

  sky::ServerPacket receivedAck =
      *tg::unpack(serverPacketPack, buffer);
  sky::Arena remoteArena;
  EXPECT_TRUE(remoteArena.applyInitializer(*receivedAck.arenaInitializer));
  EXPECT_EQ(remoteArena.getPlayer(*receivedAck.pid)->nickname, "somebody");
  EXPECT_EQ(remoteArena.motd, "test server");
}

TEST_F(ArenaTest, DeltaTest) {
  sky::Arena arena;
  arena.motd = "test server";

  tg::packInto(arenaDeltaPack, sky::ArenaDelta::Motd("secret server"), buffer);
  appLog(buffer.dump());
  arena.applyDelta(*tg::unpack(arenaDeltaPack, buffer));
  EXPECT_EQ(arena.motd, "secret server");

  sky::Player player(1);
  player.nickname = "somebody";
  tg::packInto(arenaDeltaPack, sky::ArenaDelta::Join(player), buffer);
  appLog(buffer.dump());
  arena.applyDelta(*tg::unpack(arenaDeltaPack, buffer));
  EXPECT_NE(arena.getPlayer(1), nullptr);
  EXPECT_EQ(arena.getPlayer(1)->nickname, "somebody");

  sky::PlayerDelta playerDelta = sky::PlayerDelta(std::string("somebody else"));
  tg::packInto(arenaDeltaPack, sky::ArenaDelta::Modify(1, playerDelta), buffer);
  appLog(buffer.dump());
  arena.applyDelta(*tg::unpack(arenaDeltaPack, buffer));
  EXPECT_EQ(arena.getPlayer(1)->nickname, "somebody else");

  tg::packInto(arenaDeltaPack, sky::ArenaDelta::Quit(1), buffer);
  appLog(buffer.dump());
  arena.applyDelta(*tg::unpack(arenaDeltaPack, buffer));
  EXPECT_EQ(arena.getPlayer(1), nullptr);
}

TEST_F(ArenaTest, SkyTest) {
  sky::Arena arena, remoteArena;
}
