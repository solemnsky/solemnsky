#include <gtest/gtest.h>
#include "engine/sky/skyhandle.hpp"

/**
 * Our SkyHandle subsystem operates and networks correctly.
 */
class SkyHandleTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::SkyHandle skyHandle;

  SkyHandleTest() :
      arena(sky::ArenaInit("arena", "NULL")),
      skyHandle(arena, sky::SkyHandleInit()) { }

};

/**
 * The allocation of the Sky and Environment inside a SkyHandle
 * works correctly.
 */
TEST_F(SkyHandleTest, AllocTest) {
  {
    // A player joins beforehand.
    arena.connectPlayer("Magnetic Duck");

    // Start the engine, and the environment is instantiated.
    skyHandle.start();
    ASSERT_EQ(bool(skyHandle.getEnvironment()), true);
    ASSERT_EQ(bool(skyHandle.getSky()), false);

    // After waiting for the environment to load, we can instantiate the Sky.
    skyHandle.getEnvironment()->joinWorker();
    skyHandle.instantiateSky(sky::SkyInit{});
    ASSERT_EQ(bool(skyHandle.getSky()), true);

    const sky::Sky &sky = *skyHandle.getSky();

    // A player joins after the sky is instantiated.
    arena.connectPlayer("nameless plane");

    // The sky is now connected to the arena.
    auto &player0 = *arena.getPlayer(0),
        &player1 = *arena.getPlayer(1);
    auto &particip0 = sky.getParticipation(player0),
        &particip1 = sky.getParticipation(player1);

    ASSERT_FALSE(particip0.isSpawned());
    ASSERT_FALSE(particip1.isSpawned());
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
