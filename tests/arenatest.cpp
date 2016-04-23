#include <gtest/gtest.h>
#include "sky/arena.h"

/**
 * Arena is the backbone of a multiplayer server.
 */
class ArenaTest: public testing::Test {
 public:
  ArenaTest() :
      arena(sky::ArenaInitializer("home arena", "test1")) { }

  sky::Arena arena;
};

/**
 * Players can connect, and arenas can be copied over the network
 * with ArenaInitializers.
 */
TEST_F(ArenaTest, InitializerTest) {
  arena.applyDelta(sky::ArenaDelta::Motd("some motd"));
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane");

  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(arena.getPlayer(1)->getNickname(), "nameless plane(1)");

  sky::Arena remoteArena(arena.captureInitializer());

  EXPECT_EQ(remoteArena.getName(), "home arena");
  EXPECT_EQ(remoteArena.getMotd(), "some motd");
  EXPECT_EQ(remoteArena.getMap(), "test1");
  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(remoteArena.getPlayer(1)->getNickname(), "nameless plane(1)");
}

/**
 * Arena deltas can effect various changes.
 */
TEST_F(ArenaTest, DeltaTest) {
  arena.applyDelta(
      sky::ArenaDelta::MapChange("test2")
  );

  EXPECT_EQ(arena.getMap(), "test2");

  arena.applyDelta(
      sky::ArenaDelta::Motd("new motd")
  );

  EXPECT_EQ(arena.getMotd(), "new motd");

  sky::Arena remoteArena(arena.captureInitializer());
  const auto delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");

  remoteArena.applyDelta(sky::ArenaDelta::Quit(0));

  EXPECT_EQ(remoteArena.getPlayer(0), nullptr);
}
