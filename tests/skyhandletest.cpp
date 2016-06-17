#include <gtest/gtest.h>
#include "sky/skyhandle.hpp"
#include "util/methods.hpp"

/**
 * Our SkyHandle subsystem operates and networks correctly.
 */
class SkyHandleTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::SkyHandle skyHandle;

  SkyHandleTest() :
      arena(sky::ArenaInit("arena", "NULL_MAP")),
      skyHandle(arena, sky::SkyHandleInit()) { }

};

/**
 * The allocation of the underlying sky and map is managed correctly.
 */
TEST_F(SkyHandleTest, AllocTest) {
  ASSERT_EQ(bool(skyHandle.isActive()), false);

  {
    // Starting the sky with a map.
    skyHandle.start();
    ASSERT_EQ(skyHandle.isActive(), true);
    const sky::Sky &sky = *skyHandle.sky;
    ASSERT_EQ(sky.getMap().name, "NULL_MAP");

    // Signals pass correctly.
    arena.connectPlayer("nameless plane");
    sky::Player &player = *arena.getPlayer(0);
    ASSERT_EQ(sky.getParticipation(player).isSpawned(), false);
    player.spawn({}, {300, 300}, 0);
    ASSERT_EQ(sky.getParticipation(player).isSpawned(), true);
    ASSERT_EQ(sky.getParticipation(player).plane->getState().physical.pos.x, 300);
  }

  {
    // When the map can't load, the state still makes sense.
    arena.applyDelta(sky::ArenaDelta::MapChange("map_that_cant_exist"));
    skyHandle.start();
    ASSERT_EQ(skyHandle.isActive(), false);
    ASSERT_EQ(skyHandle.loadingErrored(), true);

    arena.applyDelta(sky::ArenaDelta::MapChange("NULL_MAP"));
    skyHandle.start();
    ASSERT_EQ(skyHandle.isActive(), true);
    ASSERT_EQ(skyHandle.loadingErrored(), false);
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
    ASSERT_EQ(remoteSkyHandle.sky->getMap().name, "NULL_MAP");
    const sky::Sky &remoteSky = *remoteSkyHandle.sky;

    // The participations copied.
    sky::Player player1 = *remoteArena.getPlayer(0);
    sky::Player player2 = *remoteArena.getPlayer(1);
    ASSERT_EQ(remoteSky.getParticipation(player1).isSpawned(), true);
    ASSERT_EQ(remoteSky.getParticipation(player2).isSpawned(), false);
    ASSERT_EQ(remoteSky.getParticipation(player1).plane->
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
    ASSERT_EQ(participation.plane->getState().physical.pos.x, 300);
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

  // Correct start followed by failed restart.
  skyHandle.start();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  arena.applyDelta(sky::ArenaDelta::MapChange("map_that_doesnt_exist"));
  skyHandle.start();
  remoteSkyHandle.applyDelta(skyHandle.collectDelta());
  ASSERT_EQ(remoteSkyHandle.isActive(), false);
  ASSERT_EQ(remoteSkyHandle.loadingErrored(), false);

}


