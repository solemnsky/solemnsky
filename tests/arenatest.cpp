#include <gtest/gtest.h>
#include "sky/arena.h"

/**
 * The Arena abstraction.
 */
class ArenaTest: public testing::Test {
 public:
  tg::Packet buffer;
};

/**
 * Players can be connected.
 */
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

/**
 * ArenaDeltas work correctly, and can be packed / unpacked.
 */
TEST_F(ArenaTest, DeltaTest) {
  sky::Arena homeArena, remoteArena;

  sky::Player &player1 = homeArena.connectPlayer();
  player1.nickname = "asdf";

  tg::packInto(sky::arenaDeltaPack, sky::ArenaDelta::Join(player1), buffer);
  optional<sky::ArenaDelta> joinDelta = tg::unpack(sky::arenaDeltaPack, buffer);

  ASSERT_TRUE((bool) joinDelta);
  ASSERT_TRUE(remoteArena.applyDelta(*joinDelta));
  EXPECT_TRUE((bool) remoteArena.getPlayer(1));
  EXPECT_EQ(remoteArena.getPlayer(1)->nickname, "asdf");

  tg::packInto(sky::arenaDeltaPack, sky::ArenaDelta::Quit(1), buffer);
  optional<sky::ArenaDelta> quitDelta = tg::unpack(sky::arenaDeltaPack, buffer);

  ASSERT_TRUE((bool) quitDelta);
  ASSERT_TRUE(remoteArena.applyDelta(*quitDelta));
  EXPECT_FALSE((bool) remoteArena.getPlayer(1));
}

/**
 * Arenas can be copied through ArenaInitializer; ArenaInitializer can be
 * packed / unpacked.
 */
TEST_F(ArenaTest, InitializerTest) {
  sky::Arena homeArena, remoteArena;

  homeArena.motd = "secret arena";
  sky::Player &player1 = homeArena.connectPlayer();
  player1.nickname = "an admin";
  player1.admin = true;
  sky::Player &player2 = homeArena.connectPlayer();
  player2.nickname = "somebody else";
  homeArena.mode = sky::ArenaMode::Scoring;

  tg::packInto(sky::arenaInitializerPack,
               homeArena.captureInitializer(), buffer);
  optional<sky::ArenaInitializer> initializer =
      tg::unpack(sky::arenaInitializerPack, buffer);

  ASSERT_TRUE((bool) initializer);
  ASSERT_TRUE(remoteArena.applyInitializer(*initializer));
  EXPECT_EQ(remoteArena.motd, "secret arena");
  EXPECT_EQ(remoteArena.getPlayer(1)->nickname, "an admin");
  EXPECT_EQ(remoteArena.getPlayer(1)->admin, true);
  EXPECT_EQ(remoteArena.getPlayer(2)->nickname, "somebody else");
  EXPECT_EQ(remoteArena.mode, sky::ArenaMode::Scoring);
}
