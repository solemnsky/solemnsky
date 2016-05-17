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
      skyHandle(arena, sky::SkyHandleInitializer()) { }

  sky::Arena arena;
  sky::SkyHandle skyHandle;
};

/**
 * The allocation of the game world is managed correctly.
 */
TEST_F(SkyTest, AllocTest) {
  ASSERT_EQ(bool(skyHandle.isActive()), false);

  {
    skyHandle.start("test1");
    ASSERT_EQ(bool(skyHandle.isActive()), true);
    const sky::Sky &sky = *skyHandle.getSky();

    arena.connectPlayer("nameless plane");
    sky::Player &player = *arena.getPlayer(0);
    ASSERT_EQ(bool(sky.getParticipation(player).isSpawned()), false);
    ASSERT_NO_FATAL_FAILURE(arena.tick(1));

    player.spawn({}, {300, 300}, 0);
    ASSERT_EQ(bool(sky.getParticipation(player).isSpawned()), true);
    ASSERT_NO_FATAL_FAILURE(arena.tick(1));
  }

  skyHandle.stop();
  ASSERT_EQ(bool(skyHandle.isActive()), false);
}

/**
 * We can modify the Sky and copy the modified Sky over the network with
 * SkyInitializer.
 */
TEST_F(SkyTest, InitializerTest) {
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane 2");

  {
    sky::Player &player = *arena.getPlayer(0);
    skyHandle.start("test1");
    player.spawn({}, {300, 300}, 0);
  }

  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());
  ASSERT_EQ(remoteSkyHandle.isActive(), true);
  const sky::Sky &remoteSky = remoteSkyHandle.getSky().get();

  {
    sky::Player player1 = *remoteArena.getPlayer(0);
    sky::Player player2 = *remoteArena.getPlayer(1);
    ASSERT_EQ(remoteSky.getParticipation(player1).isSpawned(), true);
    ASSERT_EQ(remoteSky.getParticipation(player2).isSpawned(), false);
    ASSERT_EQ(skyHandle.getSky()->getParticipation(player1).getPlane()->
        getState().physical.pos.x, 300);
  }

}

/**
 * We can propagate changes over the network with a SkyDelta.
 */
TEST_F(SkyTest, DeltaTest) {
  skyHandle.start("test1");
  const sky::Sky &sky = skyHandle.getSky().get();

  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());
  const sky::Sky &remoteSky = remoteSkyHandle.getSky().get();

  auto const delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  sky::Player &player = *arena.getPlayer(0);
  player.spawn({}, {}, 0);
  player.doAction(sky::Action::Reverse, true);
  ASSERT_EQ(sky.getParticipation(player)
                .getControls().getState<sky::Action::Reverse>(), true);
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());

  {
    sky::Player &remotePlayer = *remoteArena.getPlayer(0);
    ASSERT_EQ(remoteSky.getParticipation(remotePlayer)
                  .getControls().getState<sky::Action::Reverse>(), true);
  }
}

/**
 * We can transmit a Sky instantiation through a SkyHandleDelta.
 */
TEST_F(SkyTest, DeltaAllocTest) {
  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());

  ASSERT_EQ(remoteSkyHandle.isActive(), false);

  skyHandle.start("test1");
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());

  ASSERT_EQ(remoteSkyHandle.isActive(), true);
}


