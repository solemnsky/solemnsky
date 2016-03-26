#include <gtest/gtest.h>
#include "sky/sky.h"
#include "util/methods.h"

/**
 * Our Sky render works correctly.
 */
class SkyTest: public testing::Test {
 public:
  SkyTest() { }
};

/**
 * We're attached to the arena, along for the ride.
 */
TEST_F(SkyTest, SubsystemTest) {
  sky::Arena arena(sky::ArenaInitializer("my arena"));
  sky::Sky sky(arena, sky::SkyInitializer("some map"));

  {
    sky::Player &player = arena.connectPlayer("some name");
    player.spawn({}, {300, 300}, 0);
    EXPECT_EQ(sky.getPlane(player).vital->state.pos.x, 300);
    arena.tick(0.5);
    EXPECT_NE(sky.getPlane(player).vital->state.pos.x, 300);
  }

  {
    sky::Player &player = arena.connectPlayer("some name");
    EXPECT_EQ(bool(sky.getPlane(player).vital), false);
  }
}

/**
 * Initializers work correctly.
 */
TEST_F(SkyTest, InitializerTest) {
  sky::Arena serverArena(sky::ArenaInitializer("my arena"));
  sky::Sky serverSky(serverArena, sky::SkyInitializer("some map"));

  {
    sky::Player &player0 = serverArena.connectPlayer("some guy");
    sky::Player &player1 = serverArena.connectPlayer("some other guy");
    player1.spawn({}, {200, 200}, 0);
  }

  sky::Arena clientArena(serverArena.captureInitializer());
  sky::Sky clientSky(clientArena, serverSky.captureInitializer());

  {
    sky::Player *player0(clientArena.getPlayer(0)),
        *player1(clientArena.getPlayer(1));

    ASSERT_EQ(bool(player0), true);
    ASSERT_EQ(bool(player1), true);

    sky::Plane &plane0 = clientSky.getPlane(*player0),
        &plane1 = clientSky.getPlane(*player1);
    EXPECT_EQ(bool(plane0.vital), false);
    ASSERT_EQ(bool(plane1.vital), true);
    EXPECT_EQ(plane1.vital->state.pos.x, 200);
  }
}

/**
 * Deltas work correctly.
 */
TEST_F(SkyTest, DeltaTest) {
  sky::Arena serverArena(sky::ArenaInitializer("my arena"));
  sky::Sky serverSky(serverArena, sky::SkyInitializer("some map"));

  sky::Arena clientArena(serverArena.captureInitializer());
  sky::Sky clientSky(clientArena, serverSky.captureInitializer());

  sky::ArenaDelta arenaDelta;
  {
    // server joins player
    sky::Player &player = serverArena.connectPlayer("some player");
    arenaDelta = sky::ArenaDelta::Join(player.captureInitializer());
  }

  {
    // client applies join delta
    clientArena.applyDelta(arenaDelta);
    sky::Player &newPlayer = *clientArena.getPlayer(0);
    EXPECT_EQ(bool(clientSky.getPlane(newPlayer).vital), false);
  }

  sky::SkyDelta skyDelta;
  {
    // server spawns the player
    serverArena.getPlayer(0)->spawn({}, {300, 300}, 0);
    skyDelta = serverSky.collectDelta();
  }

  {
    // client applies the sky delta
    clientSky.applyDelta(skyDelta);
    sky::Player &player = *clientArena.getPlayer(0);
    sky::Plane &plane = clientSky.getPlane(player);

    ASSERT_EQ(bool(plane.vital), true);
    EXPECT_EQ(plane.vital->state.pos.x, 300);
  }
}
