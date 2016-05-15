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
      skyManager(arena, sky::SkyInitializer()) { }

  sky::Arena arena;
  sky::SkyHandle skyManager;
};

/**
 * The allocation of the game world is managed correctly.
 */
TEST_F(SkyTest, PhysicsTest) {
  ASSERT_EQ(bool(skyManager.isActive()), false);

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  ASSERT_EQ(bool(skyManager.isActive()), true);

  const auto pid = arena.connectPlayer("nameless plane").join->pid;
  sky::Player *player = arena.getPlayer(pid);
  ASSERT_EQ(bool(skyManager.getParticipation(*player)), true);
  ASSERT_EQ(bool(skyManager.getParticipation(*player)->getPlane()), false);
  ASSERT_NO_FATAL_FAILURE(arena.tick(1));

  player->spawn({}, {300, 300}, 0);
  ASSERT_EQ(bool(skyManager.getParticipation(*player)->getPlane()), true);
  ASSERT_NO_FATAL_FAILURE(arena.tick(1));

  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Scoring));
  ASSERT_EQ(bool(skyManager.isActive()), false);
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

  ASSERT_EQ(skyManager.getParticipation(*player)->isSpawned(), true);

  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSky(remoteArena, skyManager.captureInitializer());

  ASSERT_EQ(remoteSky.getParticipation(*player)->isSpawned(), true);
}

/**
 * We can propagate changes over the network with a SkyDelta.
 */
TEST_F(SkyTest, DeltaTest) {
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSky(remoteArena, skyManager.captureInitializer());

  auto const delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  sky::Player *player = arena.getPlayer(0);
  player->spawn({}, {}, 0);
  player->doAction(sky::Action::Reverse, true);
  remoteSky.applyDelta(skyManager.collectDelta());

  ASSERT_EQ(skyManager.getParticipation(*player)
                ->getControls().getState<sky::Action::Reverse>(), true);
  ASSERT_EQ(remoteSky.getParticipation(*remoteArena.getPlayer(0))
                ->getControls().getState<sky::Action::Reverse>(), true);
}


