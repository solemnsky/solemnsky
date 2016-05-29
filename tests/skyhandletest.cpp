#include <gtest/gtest.h>
#include "sky/skyhandle.h"
#include "util/methods.h"

/**
 * Our SkyHandle subsystem operates and networks correctly.
 */
class SkyHandleTest: public testing::Test {
 public:
  SkyHandleTest() :
      arena(sky::ArenaInit("arena", "test1")),
      skyHandle(arena, sky::SkyHandleInit()) { }

  sky::Arena arena;
  sky::SkyHandle skyHandle;
};

/**
 * The allocation of the underlying sky is managed correctly.
 */
TEST_F(SkyHandleTest, AllocTest) {
  ASSERT_EQ(bool(skyHandle.isActive()), false);

  {
    // Starting the sky with a map.
    skyHandle.start();
    ASSERT_EQ(bool(skyHandle.isActive()), true);
    const sky::Sky &sky = *skyHandle.sky;
    ASSERT_EQ(sky.getMap().name, "test1");

    // Spawning is managed correctly.
    arena.connectPlayer("nameless plane");
    sky::Player &player = *arena.getPlayer(0);
    ASSERT_EQ(bool(sky.getParticipation(player).isSpawned()), false);
    player.spawn({}, {300, 300}, 0);
    ASSERT_EQ(bool(sky.getParticipation(player).isSpawned()), true);
    ASSERT_EQ(sky.getParticipation(player).getPlane()
                  ->getState().physical.pos.x, 300);
  }

  skyHandle.stop();
  ASSERT_EQ(bool(skyHandle.isActive()), false);
}

/**
 * A modified SkyHandle can be copied to a remote client with
 * SkyHandleInitializer.
 */
TEST_F(SkyHandleTest, InitializerTest) {
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane 2");

  {
    sky::Player &player = *arena.getPlayer(0);
    skyHandle.start();
    player.spawn({}, {300, 300}, 0);
    player.doAction(sky::Action::Left, true);
  }

  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());

  {
    // The sky instantiation copied through.
    ASSERT_EQ(remoteSkyHandle.isActive(), true);
    ASSERT_EQ(remoteSkyHandle.sky->getMap().name, "test1");
    const sky::Sky &remoteSky = *remoteSkyHandle.sky;

    // The participations copied.
    sky::Player player1 = *remoteArena.getPlayer(0);
    sky::Player player2 = *remoteArena.getPlayer(1);
    ASSERT_EQ(remoteSky.getParticipation(player1).isSpawned(), true);
    ASSERT_EQ(remoteSky.getParticipation(player2).isSpawned(), false);
    ASSERT_EQ(remoteSky.getParticipation(player1).getPlane()->
        getState().physical.pos.x, 300);
    ASSERT_EQ(remoteSky.getParticipation(player1).getControls()
                  .getState<sky::Action::Left>(), true);
  }
}

/**
 * We can propagate changes between SkyHandles with a SkyHandleDelta.
 */
TEST_F(SkyHandleTest, DeltaTest) {
  // Initialize: body Skies are instantiated and a player joined.
  arena.connectPlayer("nameless plane");
  skyHandle.start();
  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());

  {
    const sky::Sky &remoteSky = *remoteSkyHandle.sky;

    // Spawning and controls can be transmitted over deltas.
    sky::Player &player = *arena.getPlayer(0);
    player.spawn({}, {300, 300}, 0);
    player.doAction(sky::Action::Reverse, true);
    remoteSkyHandle.applyDelta(skyHandle.collectDelta());

    sky::Player &remotePlayer = *remoteArena.getPlayer(0);
    const auto &participation = remoteSky.getParticipation(remotePlayer);
    ASSERT_EQ(participation.getControls().getState<sky::Action::Reverse>(),
              true);
    ASSERT_EQ(participation.isSpawned(), true);
    ASSERT_EQ(participation.getPlane()->getState().physical.pos.x, 300);
  }
}

/**
 * We can transmit a Sky {de,re,}instantiation through a SkyHandleDelta.
 */
TEST_F(SkyHandleTest, DeltaAllocTest) {
  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());
  ASSERT_EQ(remoteSkyHandle.isActive(), false);

  // Starting.
  skyHandle.start();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  ASSERT_EQ(remoteSkyHandle.isActive(), true);

  // Stopping.
  skyHandle.stop();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  ASSERT_EQ(remoteSkyHandle.isActive(), false);

  // Starting followed by stopping.
  skyHandle.start();
  skyHandle.stop();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  ASSERT_EQ(remoteSkyHandle.isActive(), false);

  // Restarting.
  skyHandle.start();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  arena.applyDelta(sky::ArenaDelta::MapChange("test2"));
  skyHandle.start();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  ASSERT_EQ(remoteSkyHandle.sky->getMap().name, "test2");

}


