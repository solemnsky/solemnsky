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
  virtual ~Control() { }

  /**
   * Safe to use in any of the callback functions iff you're a
   * top-level control.
   */
  AppState *appState;

  virtual void attachState() { }
  // signal that appState is initialized

  /**
   * Signals to the app loop.
   */
  std::unique_ptr<Control> next{nullptr};
  bool quitting{false};

  /**
   * Callbacks to the app loop.
   */
  virtual void tick(float delta);
  virtual void render(Frame &f);
  virtual bool handle(const sf::Event &event);
  virtual void reset(); // reset UI elements
  // return true to prevent propogation

  virtual void signalRead();;
  virtual void signalClear();;
};

/**
 * Runs a top-level ui::Control, resulting in a full app.
 */
void runSFML(std::function<std::unique_ptr<Control>()> initCtrl);
}
