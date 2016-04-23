#include <gtest/gtest.h>
#include "sky/sky.h"
#include "util/methods.h"

/**
 * Our Sky subsystem works correctly.
 */
class SkyTest: public testing::Test {
 public:
  SkyTest() :
      arena(sky::ArenaInitializer("arena", "test1")),
      sky(arena, sky::SkyInitializer()) { }

  sky::Arena arena;
  sky::Sky sky;
};

/**
 * The map allocation is managed.
 */
TEST_F(SkyTest, MapAlloc) {
  ASSERT_EQ(bool(sky.map), false);

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));

  ASSERT_EQ(bool(sky.map), true);

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Scoring));

  ASSERT_EQ(bool(sky.map), false);
}

/**
 * We can affect the Sky, and copy the modified Sky over the network with
 * SkyInitializer.
 */
TEST_F(SkyTest, InitializerTest) {
  arena.connectPlayer("nameless plane");
  sky::Player *player = arena.getPlayer(0);
  player->spawn({}, {300, 300}, 0);

  ASSERT_EQ(sky.getPlane(*player).isSpawned(), true);

  sky::Arena remoteArena(arena.captureInitializer());
  sky::Sky remoteSky(remoteArena, sky.captureInitializer());

  ASSERT_EQ(remoteSky.getPlane(*player).isSpawned(), true);
}

/**
 * We can propogate changes over the network with a SkyDelta.
 */
TEST_F(SkyTest, DeltaTest) {
  sky::Arena remoteArena(arena.captureInitializer());
  sky::Sky remoteSky(remoteArena, sky.captureInitializer());

  // TODO: finish
}


