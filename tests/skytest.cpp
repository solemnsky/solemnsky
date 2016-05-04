#include <gtest/gtest.h>
#include "sky/sky.h"
#include "util/methods.h"

/**
 * Our Sky subsystem works correctly.
 */
class SkyTest: public testing::Test {
 public:
  SkyTest() :
      arena(sky::ArenaInit("arena", "test1")),
      sky(arena, sky::SkyInitializer()) { }

  sky::Arena arena;
  sky::Sky sky;
};

/**
 * The map allocation is managed and physics works properly.
 */
TEST_F(SkyTest, PhysicsTest) {
  ASSERT_EQ(bool(sky.map), false);

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  ASSERT_EQ(bool(sky.map), true);

  const auto pid = arena.connectPlayer("nameless plane").join->pid;
  sky::Player *player = arena.getPlayer(pid);
  ASSERT_NO_FATAL_FAILURE(arena.tick(1));
  player->spawn({}, {300, 300}, 0);
  ASSERT_NO_FATAL_FAILURE(arena.tick(1));

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Scoring));

  EXPECT_EQ(bool(sky.getPlane(*player).isSpawned()), false);
  EXPECT_EQ(bool(sky.map), false);
}

/**
 * We can affect the Sky, and copy the modified Sky over the network with
 * SkyInitializer.
 */
TEST_F(SkyTest, InitializerTest) {
  arena.connectPlayer("nameless plane");
  sky::Player *player = arena.getPlayer(0);
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  player->spawn({}, {300, 300}, 0);

  EXPECT_EQ(sky.getPlane(*player).isSpawned(), true);

  sky::Arena remoteArena(arena.captureInitializer());
  sky::Sky remoteSky(remoteArena, sky.captureInitializer());

  EXPECT_EQ(remoteSky.getPlane(*player).isSpawned(), true);
}

/**
 * We can propagate changes over the network with a SkyDelta.
 */
TEST_F(SkyTest, DeltaTest) {
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  sky::Arena remoteArena(arena.captureInitializer());
  sky::Sky remoteSky(remoteArena, sky.captureInitializer());

  auto const delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  sky::Player *player = arena.getPlayer(0);
  player->spawn({}, {}, 0);
  player->doAction(sky::Action::Reverse, true);
  remoteSky.applyDelta(sky.collectDelta());

  EXPECT_EQ(sky.getPlane(*player)
                .getControls().getState<sky::Action::Reverse>(), true);
  EXPECT_EQ(remoteSky.getPlane(*remoteArena.getPlayer(0))
                .getControls().getState<sky::Action::Reverse>(), true);
}


