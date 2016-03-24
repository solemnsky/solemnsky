#include <gtest/gtest.h>
#include "sky/sky.h"
#include "util/methods.h"

/**
 * Our Sky subsystem works correctly.
 */
class SkyTest: public testing::Test {
 public:
  SkyTest() { }
};

/**
 * We keep a plane for every player.
 */
TEST_F(SkyTest, SubsystemTest) {
  sky::Arena arena(sky::ArenaInitializer("my arena"));
  sky::Sky sky(&arena, sky::SkyInitializer("some map"));

  sky::Player &player = arena.connectPlayer("some name");
  sky.getPlane(player).spawn({}, {300, 300}, 0);
//  EXPECT_EQ(sky.getPlane(player).vital->state.pos.x, 300);
}
