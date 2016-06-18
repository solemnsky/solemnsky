#include <gtest/gtest.h>
#include "engine/sky/skyhandle.hpp"
#include "util/methods.hpp"

/**
 * Our SkyHandle subsystem operates and networks correctly.
 */
class SkyHandleTest : public testing::Test {
 public:
  sky::Arena arena;
  sky::SkyHandle skyHandle;

  SkyHandleTest() :
      arena(sky::ArenaInit("arena", "NULL")),
      skyHandle(arena, sky::SkyHandleInit()) {}

};

/**
 * The allocation of the Sky and Environment inside a SkyHandle
 * works correctly.
 */
TEST_F(SkyHandleTest, AllocTest) {
  {
    // Start the engine, and the environment is instantiated.
    skyHandle.start();
    ASSERT_EQ(bool(skyHandle.environment), true);
    ASSERT_EQ(bool(skyHandle.sky), false);

    // After waiting for the environment to load, we can instantiate the Sky.
    skyHandle.environment->waitForLoading();
    skyHandle.instantiateSky(sky::SkyInit{});
    ASSERT_EQ(bool(skyHandle.sky), true);

    const sky::Sky &sky = skyHandle.sky.get();

    // Signals pass correctly to the sky.
    arena.connectPlayer("nameless plane");
    sky::Player &player = *arena.getPlayer(0);
    ASSERT_EQ(sky.getParticipation(player).isSpawned(), false);
    player.spawn({}, {300, 300}, 0);
    ASSERT_EQ(sky.getParticipation(player).isSpawned(), true);
    ASSERT_EQ(sky.getParticipation(player).plane->getState().physical.pos.x,
              300);
  }
}

/**
 * A SkyHandle can be copied to a remote with a SkyHandleInitializer.
 */
TEST_F(SkyHandleTest, InitializerTest) {
  skyHandle.start();

  // The environment instantiation copies.
  {
    sky::Arena remoteArena{arena.captureInitializer()};
    sky::SkyHandle remoteHandle{remoteArena, skyHandle.captureInitializer()};
    ASSERT_EQ(bool(remoteHandle.environment), true);
    ASSERT_EQ(remoteHandle.environment->url, "NULL");
  }

  skyHandle.stop();

  // This is the simpler case.
  {
    sky::Arena remoteArena{arena.captureInitializer()};
    sky::SkyHandle remoteHandle{remoteArena, skyHandle.captureInitializer()};
    ASSERT_EQ(bool(remoteHandle.environment), false);
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

    skyHandle.stop();
    remoteSkyHandle.applyDelta(skyHandle.collectDelta());

    ASSERT_EQ(bool(remoteSkyHandle.environment), false);
    ASSERT_EQ(bool(remoteSkyHandle.sky), false);

    skyHandle.start();
    remoteSkyHandle.applyDelta(skyHandle.collectDelta());

    ASSERT_EQ(bool(remoteSkyHandle.environment), true);
    ASSERT_EQ(bool(remoteSkyHandle.sky), false);
  }
}
