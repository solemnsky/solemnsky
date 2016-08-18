#include <gtest/gtest.h>
#include "engine/arena.hpp"
#include "util/methods.hpp"

/**
 * The Subsystem abstraction allows us to modularize state and functionality
 * associated with an Arena.
 */
class SubsystemTest: public testing::Test {
 public:
  SubsystemTest() : arena(sky::ArenaInit("my arena", "NULL")) { }
  sky::Arena arena;
};

class LifeSubsystem: public sky::Subsystem<bool> {
 private:
  std::map<PID, bool> lives;

 protected:
  void registerPlayer(sky::Player &player) override {
    lives.emplace(player.pid, false);
    setPlayerData(player, lives.at(player.pid));
  }

  void unregisterPlayer(sky::Player &player) override {
    lives.erase(player.pid);
  }

  void onSpawn(sky::Player &player) override {
    getPlayerData(player) = true;
  }

 public:
  LifeSubsystem(sky::Arena &arena) : sky::Subsystem<bool>(arena) {
    arena.forPlayers([&](sky::Player &player) {
      registerPlayer(player);
    });
  }

  bool getLifeData(const sky::Player &player) const {
    return getPlayerData(player);
  }

  // Make a spawn happen.
  void doSpawn(sky::Player &player) {
    caller.doSpawn(player);
  }

};

class CounterSubsystem: public sky::Subsystem<float> {
 private:
  std::map<PID, TimeDiff> myData;
  LifeSubsystem &lifeSubsystem;

 protected:
  void registerPlayer(sky::Player &player) override {
    setPlayerData(
        player,
        myData.emplace(player.pid, 0).first->second);
  }

  void unregisterPlayer(sky::Player &player) override {
    myData.erase(player.pid);
  }

  void onTick(const TimeDiff delta) override {
    arena.forPlayers([&](sky::Player &player) {
      if (lifeSubsystem.getLifeData(player))
        getPlayerData(player) += delta;
    });
  }

 public:
  CounterSubsystem(sky::Arena &arena, LifeSubsystem &lifeSubsystem) :
      sky::Subsystem<float>(arena), lifeSubsystem(lifeSubsystem) {
    arena.forPlayers([&](sky::Player &player) {
      registerPlayer(player);
    });
  }

  float getTimeData(const sky::Player &player) const {
    return getPlayerData(player);
  }
};

/**
 * Subsystems can be dynamically attached.
 */
TEST_F(SubsystemTest, DynamicTest) {
  LifeSubsystem lifeSubsystem1(arena);
  arena.connectPlayer("somebody");
  auto &player = *arena.getPlayer(0);
  EXPECT_EQ(lifeSubsystem1.getLifeData(player), false);

  LifeSubsystem lifeSubsystem2(arena);
  lifeSubsystem2.doSpawn(*arena.getPlayer(0));
  EXPECT_EQ(lifeSubsystem2.getLifeData(player), true);
  EXPECT_EQ(lifeSubsystem1.getLifeData(player), true);
}

/**
 * Subsystems get passed callbacks and can store state.
 */
TEST_F(SubsystemTest, LifeCounter) {
  LifeSubsystem lifeSubsystem(arena);
  CounterSubsystem counterSubsystem(arena, lifeSubsystem);

  arena.connectPlayer("player number 1");
  auto &player1 = *arena.getPlayer(0);
  EXPECT_EQ(lifeSubsystem.getLifeData(player1), false);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0);
  arena.tick(0.5);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0);
  lifeSubsystem.doSpawn(player1);
  EXPECT_EQ(lifeSubsystem.getLifeData(player1), true);
  arena.tick(0.5);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0.5);
}


