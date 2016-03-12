#include <gtest/gtest.h>
#include "sky/arena.h"

/**
 * The Arena abstraction.
 */
class ArenaTest: public testing::Test {
 public:
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
 * ArenaDeltas work correctly.
 */
TEST_F(ArenaTest, DeltaTest) {
  sky::Arena homeArena, remoteArena;

  sky::Player &player1 = homeArena.connectPlayer();
  player1.nickname = "asdf";

  remoteArena.applyDelta(sky::ArenaDelta::Join(player1));
  EXPECT_TRUE((bool) remoteArena.getPlayer(1));
  EXPECT_EQ(remoteArena.getPlayer(1)->nickname, "asdf");

  remoteArena.applyDelta(sky::ArenaDelta::Quit(1));
  EXPECT_FALSE((bool) remoteArena.getPlayer(1));
}

/**
 * Arenas can be copied through ArenaInitializer.
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

  remoteArena.applyInitializer(homeArena.captureInitializer());
  EXPECT_EQ(remoteArena.motd, "secret arena");
  EXPECT_EQ(remoteArena.getPlayer(1)->nickname, "an admin");
  EXPECT_EQ(remoteArena.getPlayer(1)->admin, true);
  EXPECT_EQ(remoteArena.getPlayer(2)->nickname, "somebody else");
  EXPECT_EQ(remoteArena.mode, sky::ArenaMode::Scoring);
}
