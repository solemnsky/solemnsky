#include <gtest/gtest.h>
#include "sky/sky.h"
#include "util/methods.h"

/**
 * Our Sky game engine works correctly.
 */
class SkyTest: public testing::Test {
 public:
  SkyTest() { }
};

TEST_F(SkyTest, InitializerTest) {
  sky::Sky serverSky("map");

  serverSky.addPlane(0, {}, {100, 100}, 0);

  sky::Sky clientSky(serverSky.captureInitializer());
  EXPECT_TRUE(bool(clientSky.getPlane(0)));
  EXPECT_EQ(clientSky.mapName, "map");
  EXPECT_EQ(clientSky.getPlane(0)->state.pos.x, 100);
}

TEST_F(SkyTest, DeltaTest) {
  sky::Sky serverSky("map"), clientSky("map");

  serverSky.addPlane(0, {}, {100, 100}, 0);
  clientSky.applyDelta(serverSky.collectDelta());
  EXPECT_TRUE(bool(clientSky.getPlane(0)));

  serverSky.removePlane(0);
  clientSky.applyDelta(serverSky.collectDelta());
  EXPECT_FALSE(bool(clientSky.getPlane(0)));
}

