#include <gtest/gtest.h>
#include "sky/scoreboard.h"

/**
 * The scoreboard subsystem operates and networks properly.
 */
class ScoreboardTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::Scoreboard scoreboard;

  ScoreboardTest() :
      arena(sky::ArenaInit("special arena", "test", sky::ArenaMode::Lobby)),
      scoreboard(arena,
                 sky::ScoreboardInit({"kills", "deaths", "assists"})) { }

};

/**
 * We can alter a scoreboard and copy it to a remote.
 */
TEST_F(ScoreboardTest, CopyTest) {
  arena.connectPlayer("nameless plane");
  sky::Player &player1 = *arena.getPlayer(0);
  ASSERT_EQ(scoreboard.getScore(player1).getValueAt(0), 0);
  scoreboard.getScore(player1).setValueAt(0, 5);
  ASSERT_EQ(scoreboard.getScore(player1).getValueAt(0), 5);

  sky::Arena remoteArena(arena.captureInitializer());
  sky::Scoreboard remoteScoreboard(remoteArena,
                                   scoreboard.captureInitializer());
  sky::Player &remotePlayer1 = *remoteArena.getPlayer(0);
  ASSERT_EQ(remoteScoreboard.getFields()[1], "deaths");
  ASSERT_EQ(remoteScoreboard.getScore(remotePlayer1).getValueAt(0), 5);
}

/**
 * We can transmit changes to a remote.
 */
TEST_F(ScoreboardTest, DeltaTest) {
  sky::Arena remoteArena(arena.captureInitializer());
  sky::Scoreboard remoteScoreboard(remoteArena,
                                   scoreboard.captureInitializer());

  const auto delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);

  // scoreboard has no useful delta to transmit
  ASSERT_EQ(bool(scoreboard.collectDelta()), false);

  {
    sky::Player &player = *arena.getPlayer(0);
    scoreboard.getScore(player).setValueAt(0, 10);
    scoreboard.getScore(player).setValueAt(1, 5);
  }

  {
    // scoreboard has delta to transmit, and it has the desired effect
    remoteScoreboard.applyDelta(scoreboard.collectDelta().get());
    sky::Player &player = *remoteArena.getPlayer(0);
    ASSERT_EQ(remoteScoreboard.getScore(player).getValueAt(0), 10);
    ASSERT_EQ(remoteScoreboard.getScore(player).getValueAt(1), 5);
  }
}

