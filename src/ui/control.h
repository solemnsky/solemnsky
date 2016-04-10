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
  Profiler(int size);

  RollingSampler cycleTime;
  RollingSampler logicTime;
  RollingSampler renderTime;
  RollingSampler primCount;
};

struct ProfilerSnapshot {
  ProfilerSnapshot() = default; // 'null' defaults, all zeroes
  ProfilerSnapshot(const Profiler &profiler);

  SamplerSnapshot cycleTime, logicTime, renderTime;
};

/**
 * Various lower level settings and SFML-provided state for us to access
 * occasionally.
 */
struct AppState {
  AppState(const sf::RenderWindow &window,
           const Profiler &profiler,
           const float &time);

  const float &time;
  const sf::RenderWindow &window;
  const Profiler &profiler;

  float timeSince(const float event) const;
};

/**
 * The control abstraction: a tangible GUI entity.
 */
class Control {
 private:
 protected:
  AppState &appState;

  std::vector<Control *> children;

  inline void areChildren(std::initializer_list<Control *> controls) {
    for (auto control : controls) children.push_back(control);
  }

 public:
  Control(AppState &appState);
  virtual ~Control() { }

  // flags
  std::unique_ptr<Control> next;
  bool quitting;

  // callbacks
  virtual void tick(float delta);
  virtual void render(Frame &f);
  virtual bool handle(const sf::Event &event);
  virtual void reset();

  // signals
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
  ui::Profiler profiler;
  Cooldown resizeCooldown;

  const float tickStep;
  float rollingTickTime;
  float time;
  AppState appState;
  std::unique_ptr<Control> ctrl;

  sf::Clock cycleClock, profileClock;

  // app loop
  void tick();
  void handle();
  void renderAndSleep();

 public:
  ControlExec(std::function<std::unique_ptr<Control>(AppState &)> initCtrl);

  void run();

};

void runSFML(std::function<std::unique_ptr<Control>(AppState &)> initCtrl);
}
