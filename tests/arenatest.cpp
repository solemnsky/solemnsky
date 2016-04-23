#include <gtest/gtest.h>
#include "sky/arena.h"

/**
 * Arena is the backbone of a multiplayer server.
 */
class ArenaTest: public testing::Test { };

/**
 * Players can connect, and arenas can be copied over the network
 * with ArenaInitializers.
 */
TEST_F(ArenaTest, InitializerTest) {
  sky::Arena homeArena(sky::ArenaInitializer("home arena", "test1"));
  homeArena.applyDelta(sky::ArenaDelta::Motd("some motd"));
  homeArena.connectPlayer("nameless plane");
  homeArena.connectPlayer("nameless plane");

  EXPECT_EQ(homeArena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(homeArena.getPlayer(1)->getNickname(), "nameless plane(1)");

  sky::Arena remoteArena(homeArena.captureInitializer());

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
  sky::Arena homeArena(sky::ArenaInitializer("home arena", "test1"));

  homeArena.applyDelta(
      sky::ArenaDelta::MapChange("test2")
  );

  EXPECT_EQ(homeArena.getMap(), "test2");

  homeArena.applyDelta(
      sky::ArenaDelta::Motd("new motd")
  );

  EXPECT_EQ(homeArena.getMotd(), "new motd");

  sky::Arena remoteArena(homeArena.captureInitializer());
  const auto delta = homeArena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");

  remoteArena.applyDelta(sky::ArenaDelta::Quit(0));

  EXPECT_EQ(remoteArena.getPlayer(0), nullptr);
}
