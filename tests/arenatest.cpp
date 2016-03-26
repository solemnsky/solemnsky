#include <gtest/gtest.h>
#include "sky/arena.h"
#include "util/methods.h"

/**
 * Arena is the backbone of a multiplayer server.
 */
class ArenaTest: public testing::Test { };

/**
 * Players can be connected and nicknames / PIDs are allocated correctly.
 */
TEST_F(ArenaTest, ConnectionTest) {
  sky::Arena arena(sky::ArenaInitializer("home sweet home"));

  sky::Player &player1 = arena.connectPlayer("asdf");
  sky::Player &player2 = arena.connectPlayer("asdf");
  sky::Player &player3 = arena.connectPlayer("asdf");

  EXPECT_EQ(player1.pid, 0);
  EXPECT_EQ(player2.pid, 1);
  EXPECT_EQ(player3.pid, 2);
  EXPECT_EQ(player1.nickname, "asdf");
  EXPECT_EQ(player2.nickname, "asdf(1)");
  EXPECT_EQ(player3.nickname, "asdf(2)");

  arena.quitPlayer(player1);
  EXPECT_EQ(arena.getPlayer(0), nullptr);

  sky::Player &player4 = arena.connectPlayer("asdf");
  EXPECT_EQ(player4.nickname, "asdf");
}

/**
 * Arenas can be copied through ArenaInitializer.
 */
TEST_F(ArenaTest, InitializerTest) {
  sky::Arena serverArena(sky::ArenaInitializer("test arena"));
  serverArena.motd = "secret arena";
  {
    sky::Player &player1 = serverArena.connectPlayer("an admin");
    player1.admin = true;
    sky::Player &player2 = serverArena.connectPlayer("somebody else");
    serverArena.mode = ArenaMode::Scoring;
  }

  sky::Arena clientArena = serverArena.captureInitializer();
  EXPECT_EQ(clientArena.motd, "secret arena");
  EXPECT_EQ(clientArena.getPlayer(0)->nickname, "an admin");
  EXPECT_EQ(clientArena.getPlayer(0)->admin, true);
  EXPECT_EQ(clientArena.getPlayer(1)->nickname, "somebody else");
  EXPECT_EQ(clientArena.mode, ArenaMode::Scoring);
}

/**
 * ArenaDeltas work correctly.
 */
TEST_F(ArenaTest, DeltaTest) {
  sky::ArenaInitializer init("arena world");
  sky::Arena clientArena(init), serverArena(init);

  sky::Player &serverPlayer = serverArena.connectPlayer("asdf");
  serverPlayer.admin = true;

  clientArena.applyDelta(
      sky::ArenaDelta::Join(serverPlayer.captureInitializer()));
  ASSERT_EQ(bool(clientArena.getPlayer(0)), true);
  sky::Player &clientPlayer = *clientArena.getPlayer(0);
  EXPECT_EQ(clientPlayer.nickname, "asdf");

  sky::PlayerDelta delta = serverPlayer.zeroDelta();
  delta.team.emplace(1);
  clientArena.applyDelta(
      sky::ArenaDelta::Delta(0, delta));
  EXPECT_EQ(clientPlayer.team, 1);

  clientArena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(bool(clientArena.getPlayer(0)), false);
}

