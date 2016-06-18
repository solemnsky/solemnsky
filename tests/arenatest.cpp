#include <gtest/gtest.h>
#include "engine/arena.hpp"

/**
 * Arena is the backbone of a multiplayer server.
 */
class ArenaTest : public testing::Test {
 public:
  ArenaTest() :
      arena(sky::ArenaInit("home arena", "NULL_MAP")) {}

  sky::Arena arena;
};

/**
 * Player connections and disconnections are managed correctly.
 */
TEST_F(ArenaTest, ConnectionTest) {
  // Join and quit.
  arena.connectPlayer("nameless plane");
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");
  arena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(arena.getPlayer(0), nullptr);

  // Reusing PIDs.
  arena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(arena.connectPlayer("nameless plane").join->pid, PID(0));
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");

}

/**
 * Player nicknames are allocated correctly.
 */
TEST_F(ArenaTest, NicknameTest) {
  // Nickname qualification.
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless somebody");
  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane");
  EXPECT_EQ(arena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(arena.getPlayer(1)->getNickname(), "nameless somebody");
  EXPECT_EQ(arena.getPlayer(2)->getNickname(), "nameless plane(1)");
  EXPECT_EQ(arena.getPlayer(3)->getNickname(), "nameless plane(2)");

  // Requesting nickname change.
  EXPECT_EQ(arena.allocNewNickname(*arena.getPlayer(3), "nameless plane"),
            "nameless plane(2)");
  EXPECT_EQ(arena.allocNewNickname(*arena.getPlayer(3), "nameless somebody"),
            "nameless somebody(1)");
  EXPECT_EQ(arena.allocNewNickname(*arena.getPlayer(0), "nameless plane"),
            "nameless plane(3)");

}

/**
 * Arena deltas can effect various changes.
 */
TEST_F(ArenaTest, DeltaTest) {
  // Map, mode and motd.
  arena.applyDelta(sky::ArenaDelta::EnvChange("test2"));
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  arena.applyDelta(sky::ArenaDelta::Motd("new motd"));

  EXPECT_EQ(arena.getNextEnv(), "test2");
  EXPECT_EQ(arena.getMode(), sky::ArenaMode::Game);
  EXPECT_EQ(arena.getMotd(), "new motd");

  // Player connection.
  sky::Arena remoteArena(arena.captureInitializer());
  const auto delta = arena.connectPlayer("nameless plane");
  remoteArena.applyDelta(delta);
  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");

  // Player disconnection.
  remoteArena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_EQ(remoteArena.getPlayer(0), nullptr);

}

/**
 * PlayerDeltas can be used to change Player records.
 */
TEST_F(ArenaTest, PlayerDeltaTest) {
  arena.connectPlayer("nameless plane");
  sky::Player &player = *arena.getPlayer(0);

  {
    // Single deltas work.
    sky::PlayerDelta delta{player};
    delta.latencyStats.emplace(50, 60);
    delta.admin = true;
    delta.envLoaded = true;

    EXPECT_EQ(player.getLatency(), 0);
    EXPECT_EQ(player.getClockOffset(), 0);
    EXPECT_EQ(player.isAdmin(), false);
    EXPECT_EQ(player.hasLoadedEnv(), false);
    EXPECT_EQ(player.latencyIsCalculated(), false);

    arena.applyDelta(sky::ArenaDelta::Delta(0, delta));

    EXPECT_EQ(player.getLatency(), 50);
    EXPECT_EQ(player.getClockOffset(), 60);
    EXPECT_EQ(player.isAdmin(), true);
    EXPECT_EQ(player.hasLoadedEnv(), true);
    EXPECT_EQ(player.latencyIsCalculated(), true);

    // Out-of-bounds deltas don't break anything.
    EXPECT_NO_THROW(arena.applyDelta(sky::ArenaDelta::Delta(1, delta)));
  }

  // Multiple deltas have multiple effects.
  arena.connectPlayer("nameless plane 2");
  std::map<PID, sky::PlayerDelta> deltas;
  {
    sky::PlayerDelta delta{*arena.getPlayer(0)};
    delta.team.emplace(1);
    deltas.emplace(0, delta);
  }
  {
    sky::PlayerDelta delta{*arena.getPlayer(1)};
    delta.team.emplace(1);
    deltas.emplace(1, delta);
  }
  arena.applyDelta(sky::ArenaDelta::Delta(deltas));
  EXPECT_EQ(arena.getPlayer(0)->getTeam(), 1);
  EXPECT_EQ(arena.getPlayer(1)->getTeam(), 1);

}

/**
 * A modified Arena can be copied to a new client with an ArenaInit.
 */
TEST_F(ArenaTest, InitializerTest) {
  arena.applyDelta(sky::ArenaDelta::Motd("some motd"));
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));

  arena.connectPlayer("nameless plane");
  arena.connectPlayer("nameless plane");
  {
    sky::PlayerDelta delta;
    delta.envLoaded = true;
    delta.team = 2;
    arena.getPlayer(0)->applyDelta(delta);
  }

  sky::Arena remoteArena(arena.captureInitializer());

  EXPECT_EQ(remoteArena.getName(), "home arena");
  EXPECT_EQ(remoteArena.getNextEnv(), "NULL_MAP");
  EXPECT_EQ(remoteArena.getMotd(), "some motd");
  EXPECT_EQ(remoteArena.getMode(), sky::ArenaMode::Game);
  EXPECT_EQ(remoteArena.getPlayer(0)->getNickname(), "nameless plane");
  EXPECT_EQ(remoteArena.getPlayer(0)->hasLoadedEnv(), true);
  EXPECT_EQ(remoteArena.getPlayer(0)->getTeam(), 2);
  EXPECT_EQ(remoteArena.getPlayer(1)->getNickname(), "nameless plane(1)");
}

