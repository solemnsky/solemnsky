#include <gtest/gtest.h>
#include "sky/arena.h"

/**
 * Arena is the backbone of a multiplayer server.
 */
class ArenaTest: public testing::Test {
 public:
  ArenaTest() :
      arena(sky::ArenaInit("home arena", "test1")) { }

  sky::Arena arena;
};

/**
 * Connections / disconnections are managed correctly, along with nicknames
 * and PIDs.
 */
TEST_F(ArenaTest, ConnectionTest) {
  // Join and quit.
  arena.connectPlayer("nameless plane");
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");
  arena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(arena.getPlayer(0), nullptr);

  // Nickname qualification.
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane");
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(arena.getPlayer(1)->getNickname(), "nameless plane(1)");

  // Reusing PIDs.
  arena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(arena.connectPlayer("nameless plane").join->pid, 0);
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");

  // Nickname qualification.
  arena.connectPlayer("nameless plane");
  EXPECT_EQ(arena.getPlayer(2)->getNickname(), "nameless plane(2)");
}

/**
 * Arena deltas can effect various changes.
 */
TEST_F(ArenaTest, DeltaTest) {
  // Map, mode and motd.
  arena.applyDelta(sky::ArenaDelta::MapChange("test2"));
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  arena.applyDelta(sky::ArenaDelta::Motd("new motd"));

  EXPECT_EQ(arena.getMap(), "test2");
  EXPECT_EQ(arena.getMode(), sky::ArenaMode::Game);
  EXPECT_EQ(arena.getMotd(), "new motd");

  // Player connection.
  sky::Arena remoteArena(arena.captureInitializer());
  const auto delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);
  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");

  // Player disconnection.
  remoteArena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(remoteArena.getPlayer(0), nullptr);
}

/**
 * A modified Arena can be copied to a new client with a SkyInitializer.
 */
TEST_F(ArenaTest, InitializerTest) {
  arena.applyDelta(sky::ArenaDelta::Motd("some motd"));
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane");

  sky::Arena remoteArena(arena.captureInitializer());

  EXPECT_EQ(remoteArena.getName(), "home arena");
  EXPECT_EQ(remoteArena.getMap(), "test1");
  EXPECT_EQ(remoteArena.getMotd(), "some motd");
  EXPECT_EQ(remoteArena.getMode(), sky::ArenaMode::Game);
  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(remoteArena.getPlayer(1)->getNickname(), "nameless plane(1)");
}

