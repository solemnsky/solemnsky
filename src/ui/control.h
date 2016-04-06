/**
 * The core of our UI abstraction.
 */
#pragma once
#include <SFML/Graphics.hpp>
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
           const Profiler &profiler) :
      window(window), profiler(profiler) { }

  const sf::RenderWindow &window;
  const Profiler &profiler;
};

class Control {
 private:
 protected:
  std::vector<Control *> children;

  inline void areChildren(std::initializer_list<Control *> controls) {
    for (auto control : controls) children.push_back(control);
  }

 public:
  Control();
  virtual ~Control() { }

  AppState *appState;

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
void runSFML(std::function<std::unique_ptr<Control>()> initCtrl);
}
