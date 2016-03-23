#include <gtest/gtest.h>
#include "sky/arena.h"
#include "sky/event.h"

/**
 * The Arena abstraction.
 */
class ArenaTest: public testing::Test {
 public:
};

/**
 * Players can be connected and nicknames / PIDs are allocated correctly.
 */
TEST_F(ArenaTest, ConnectionTest) {
  sky::Arena arena;
  sky::Player &player1 = arena.joinPlayer("asdf");
  EXPECT_EQ(player1.pid, 0);
  sky::Player &player2 = arena.joinPlayer("asdf");
  EXPECT_EQ(player2.pid, 1);
  sky::Player &player3 = arena.joinPlayer("asdf");
  EXPECT_EQ(player2.pid, 1);

  EXPECT_EQ(arena.getPlayer(0)->nickname, "asdf");
  EXPECT_EQ(arena.getPlayer(1)->nickname, "asdf(1)");
  EXPECT_EQ(arena.getPlayer(2)->nickname, "asdf(2)");

  arena.quitPlayer(player1);
  EXPECT_EQ(arena.getPlayer(0), nullptr);

  sky::Player &player4 = arena.joinPlayer("fdsa");
  EXPECT_EQ(arena.getPlayer(0)->nickname, "fdsa");
}

/**
 * ArenaDeltas work correctly.
 */
TEST_F(ArenaTest, DeltaTest) {
  sky::Arena clientArena, serverArena;

  sky::Player &player1 = serverArena.joinPlayer("asdf");

  clientArena.applyDelta(sky::ArenaDelta::Join(0, player1));
  EXPECT_TRUE((bool) clientArena.getPlayer(0));
  EXPECT_EQ(clientArena.getPlayer(0)->nickname, "asdf");

  clientArena.applyDelta(sky::ArenaDelta::Quit(0));
  EXPECT_FALSE((bool) clientArena.getPlayer(0));
}

/**
 * Arenas can be copied through ArenaInitializer.
 */
TEST_F(ArenaTest, InitializerTest) {
  sky::Arena clientArena, serverArena;

  serverArena.motd = "secret arena";
  sky::Player &player1 = serverArena.joinPlayer("an admin");
  player1.admin = true;
  sky::Player &player2 = serverArena.joinPlayer("somebody else");
  serverArena.mode = sky::ArenaMode::Scoring;

  clientArena.applyInitializer(serverArena.captureInitializer());
  EXPECT_EQ(clientArena.motd, "secret arena");
  EXPECT_EQ(clientArena.getPlayer(0)->nickname, "an admin");
  EXPECT_EQ(clientArena.getPlayer(0)->admin, true);
  EXPECT_EQ(clientArena.getPlayer(1)->nickname, "somebody else");
  EXPECT_EQ(clientArena.mode, sky::ArenaMode::Scoring);
}

/**
 * Arenas correctly manage their Skies.
 */
TEST_F(ArenaTest, SkyTest) {
  sky::Arena clientArena, serverArena;

  serverArena.mode = sky::ArenaMode::Game;
  serverArena.sky.emplace("some map");

  clientArena.applyInitializer(serverArena.captureInitializer());
  EXPECT_TRUE(bool(clientArena.sky));

  clientArena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Scoring));
  EXPECT_FALSE(bool(clientArena.sky));
}

/**
 * Arenas give ClientEvents on delta application; for clients.
 */
TEST_F(ArenaTest, EventTest) {
  sky::Arena clientArena, serverArena;
  sky::Player &player = serverArena.joinPlayer("nickname");

  {
    optional<sky::ClientEvent> event =
        clientArena.applyDelta(sky::ArenaDelta::Join(0, player));

    ASSERT_TRUE((bool) event);
    EXPECT_EQ(*event->name, "nickname");
    EXPECT_EQ(event->type, sky::ClientEvent::Type::Join);
  }

  {
    sky::PlayerDelta playerDelta(player);
    playerDelta.nickname = "new nickname";
    optional<sky::ClientEvent> event =
        clientArena.applyDelta(sky::ArenaDelta::Delta(0, playerDelta));

    ASSERT_TRUE((bool) event);
    EXPECT_EQ(*event->newName, "new nickname");
    EXPECT_EQ(*event->name, "nickname");
    EXPECT_EQ(event->type, sky::ClientEvent::Type::NickChange);
  }
}
