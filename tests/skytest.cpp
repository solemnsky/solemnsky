#include <gtest/gtest.h>
#include "sky/sky.h"

/**
 * The Sky subsystem operates and networks correctly.
 */
class SkyTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::Sky sky;

  SkyTest() :
      arena(sky::ArenaInit("special arena", "test1", sky::ArenaMode::Lobby)),
      sky(arena, sky::SkyInit("test1")) { }

};

/**
 * ParticipationInput allows players limited authority over a remote game state.
 */
TEST_F(SkyTest, InputTest
) {
  arena.connectPlayer("nameless plane");
auto &player = *arena.getPlayer(0);
auto &participation = sky.getParticipation(player);

player.spawn({
}, {
200, 200}, 0);

  {
sky::ParticipationInput input;
input.physical.
emplace(sky::PhysicalState({300, 300}, {}, 50, 0));
participation.
applyInput(input);
  }

ASSERT_EQ(participation
.
getPlane() -> getState()
.physical.pos.x, 300);
ASSERT_EQ(participation
.
getPlane() -> getState()
.physical.rot, 50);

}


