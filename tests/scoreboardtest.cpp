#include <gtest/gtest.h>
#include "sky/scoreboard.h"

/**
 * Our multiplayer protocol verbs encode all network communication, and are
 * serialized through cereal.
 */
class ScoreboardTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::Scoreboard scoreboard;

  ScoreboardTest() :
      arena(sky::ArenaInit("special arena", "test1", sky::ArenaMode::Lobby)),
      scoreboard(arena,
                 sky::ScoreboardInit({"kills", "deaths", "assists"})) { }

};

/**
 * We can alter a scoreboard and copy it with a ScoreboardInit.
 */
TEST_F(ScoreboardTest, CopyTest) {
arena.connectPlayer("nameless guy");
}
