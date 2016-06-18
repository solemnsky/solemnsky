#include <gtest/gtest.h>
#include "engine/sky/skyhandle.hpp"

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
    ASSERT_EQ(bool(skyHandle.getEnvironment()), true);
    ASSERT_EQ(bool(skyHandle.getSky()), false);

    // After waiting for the environment to load, we can instantiate the Sky.
    skyHandle.getEnvironment()->waitForLoading();
    skyHandle.instantiateSky(sky::SkyInit{});
    ASSERT_EQ(bool(skyHandle.getSky()), true);

    const sky::Sky &sky = *skyHandle.getSky();

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
    ASSERT_EQ(bool(remoteHandle.getEnvironment()), true);
    ASSERT_EQ(remoteHandle.getEnvironment()->url, "NULL");
  }

  skyHandle.stop();

  // A lack of environment instantiation also copies.
  {
    sky::Arena remoteArena{arena.captureInitializer()};
    sky::SkyHandle remoteHandle{remoteArena, skyHandle.captureInitializer()};
    ASSERT_EQ(bool(remoteHandle.getEnvironment()), false);
  }

}

/**
 * We can propagate changes between SkyHandles with a SkyHandleDelta.
 */
TEST_F(SkyHandleTest, DeltaTest) {
  arena.connectPlayer("nameless plane");
  skyHandle.start();
  sky::Arena remoteArena(arena.captureInitializer());
  sky::SkyHandle remoteSkyHandle(remoteArena, skyHandle.captureInitializer());

  {
    // We stop the SkyHandle and apply the delta.
    skyHandle.stop();
    remoteSkyHandle.applyDelta(skyHandle.collectDelta().get());
    ASSERT_EQ(bool(skyHandle.collectDelta()), false);

    ASSERT_EQ(bool(remoteSkyHandle.getEnvironment()), false);
    ASSERT_EQ(bool(remoteSkyHandle.getSky()), false);

    // We start the SkyHandle and apply the delta.
    skyHandle.start();
    remoteSkyHandle.applyDelta(skyHandle.collectDelta().get());
    ASSERT_EQ(bool(skyHandle.collectDelta()), false);

    ASSERT_EQ(bool(remoteSkyHandle.getEnvironment()), true);
    ASSERT_EQ(bool(remoteSkyHandle.getSky()), false);
  }
}
