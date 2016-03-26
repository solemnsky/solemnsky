#include <gtest/gtest.h>
#include "sky/arena.h"
#include "util/methods.h"

/**
 * The Subsystem abstraction.
 */
class SubsystemTest: public testing::Test { };

class LifeSubsystem: public sky::Subsystem {
 private:
  std::map<PID, bool> lives;

 protected:
  void registerPlayer(sky::Player &player) override {
    lives.emplace(player.pid, false);
    player.data.push_back(&lives.at(player.pid));
  }

  void unregisterPlayer(sky::Player &player) override {
    lives.erase(player.pid);
  }

  void onSpawn(sky::Player &player, const sky::PlaneTuning &,
               const sf::Vector2f &, const float) override {
    getPlayerData<bool>(player) = true;
  }

  void onAction(sky::Player &player, const sky::Action &action) {
    if (action.type == sky::Action::Type::Suicide)
      getPlayerData<bool>(player) = false;
  }
 public:
  LifeSubsystem(sky::Arena &arena) : sky::Subsystem(arena) {
    for (auto &player : arena.players) registerPlayer(player.second);
  }

  bool getLifeData(const sky::Player &player) {
    return getPlayerData<bool>(player);
  }
};

class CounterSubsystem: public sky::Subsystem {
 private:
  std::map<PID, float> myData;
  LifeSubsystem &lifeSubsystem;

 protected:
  void registerPlayer(sky::Player &player) override {
    myData.emplace(player.pid, 0);
    player.data.push_back(&myData.at(player.pid));
  }

  void unregisterPlayer(sky::Player &player) override {
    myData.erase(player.pid);
  }

  void onTick(const float delta) override {
    for (auto &pair: arena.players) {
      sky::Player &player = pair.second;
      if (lifeSubsystem.getLifeData(player))
        getPlayerData<float>(player) += delta;
    }
  }

 public:
  CounterSubsystem(sky::Arena &arena, LifeSubsystem &lifeSubsystem) :
      sky::Subsystem(arena), lifeSubsystem(lifeSubsystem) {
    for (auto &player : arena.players) registerPlayer(player.second);
  }

  float getTimeData(const sky::Player &player) {
    return getPlayerData<float>(player);
  }
};

TEST_F(SubsystemTest, LifeCounter) {
  sky::Arena arena(sky::ArenaInitializer("my arena"));
  LifeSubsystem lifeSubsystem(arena);
  CounterSubsystem counterSubsystem(arena, lifeSubsystem);

  sky::Player &player1 = arena.connectPlayer("player number 1");
  EXPECT_EQ(lifeSubsystem.getLifeData(player1), false);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0);
  arena.tick(0.5);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0);
  player1.spawn({}, {300, 300}, 0);
  EXPECT_EQ(lifeSubsystem.getLifeData(player1), true);
  arena.tick(0.5);
  EXPECT_EQ(counterSubsystem.getTimeData(player1), 0.5);
}


