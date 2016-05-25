/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * The core of our UI abstraction.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "util/telegraph.h"
#include <vector>
#include <stack>
#include <functional>
#include <memory>
#include "frame.h"
#include "util/types.h"
#include "signal.h"

namespace ui {

/**
 * Manager for performance profile data collected by the game loop.
 */
struct Profiler {
  Profiler(const unsigned int size);

  RollingSampler cycleTime;
  RollingSampler logicTime;
  RollingSampler renderTime;
  RollingSampler primCount;
};

struct ProfilerSnapshot {
  ProfilerSnapshot() = default; // 'null' defaults, all zeroes
  ProfilerSnapshot(const Profiler &profiler);

  TimeStats cycleTime, logicTime, renderTime;
};

/**
 * Various lower level settings and SFML-provided state for us to access
 * occasionally.
 */
struct AppState {
  AppState(const sf::RenderWindow &window,
           const Profiler &profiler,
           const sf::Time &time);

  const sf::Time &uptime;
  const sf::RenderWindow &window;
  const Profiler &profiler;

  double timeSince(const double event) const;
};

/**
 * The control abstraction: a tangible GUI entity.
 */
class Control {
 protected:
  AppState &appState;
  std::vector<Control *> children;

  void areChildren(std::initializer_list<Control *> controls);
  
 public: 
  Control(AppState &appState);
  virtual ~Control() { }

  // Quitting flag.
  bool quitting;

  // Callbacks.
  virtual void poll(const float delta);
  virtual void tick(const float delta);
  virtual void render(Frame &f);
  virtual bool handle(const sf::Event &event);
  virtual void reset();

  // Signal callbacks.
  virtual void signalRead();
  virtual void signalClear();

};

/**
 * Runs a top-level ui::Control, resulting in a full app.
 */
class ControlExec {
 private:
  tg::UsageFlag flag;

  static sf::ContextSettings makeSettings();
  sf::RenderWindow window;
  Frame frame;
  Cooldown resizeCooldown;
  sf::Time uptime;

  ui::Profiler profiler;
  const float tickStep;
  float rollingTickTime;
  sf::Clock cycleClock, profileClock;

  AppState appState;
  std::unique_ptr<Control> ctrl;

  // App loop submethods.
  void tick();
  void handle();
  void renderAndSleep();

 public:
  ControlExec(std::function<std::unique_ptr<Control>(AppState &)> initCtrl);

  void run();

};

void runSFML(std::function<std::unique_ptr<Control>(AppState &)> initCtrl);

}

