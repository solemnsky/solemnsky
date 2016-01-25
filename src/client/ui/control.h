/**
 * The concept of a Control object is the basis of our UI abstraction.
 * Nothing interesting or tricky here, just an interface that supports ticking
 * with time, rendering to a ui::Frame, handling user input events from
 * SFML, and giving its place to another Control.
 */
#ifndef SOLEMNSKY_CONTROL_H
#define SOLEMNSKY_CONTROL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <functional>
#include <memory>
#include "frame.h"
#include "profiler.h"
#include "signal.h"

namespace ui {

/**
 * Various lower level settings and SFML-provided state for us to access
 * occasionally.
 */
struct AppState {
  AppState(sf::RenderWindow *const window,
           Profiler *const profiler) :
      window(window), profiler(profiler) { }

  sf::RenderWindow *const window;
  Profiler *const profiler;
};

class Control {
private:
public:
  virtual ~Control() { }

  /**
   * SFML window, safe to use in any of the callback functions if you're a
   * top-level control. This feels kind of hacky, but it'll have to do.
   */
  AppState *appState;

  virtual void attachState() { }

  /**
   * Signals to the app loop.
   */
  std::unique_ptr<Control> next{nullptr};
  bool quitting{false};

  /**
   * Callbacks to the app loop.
   */
  virtual void tick(float delta) = 0;
  virtual void render(Frame &f) = 0;
  virtual void handle(const sf::Event &event) = 0;

  /**
   * Signal framework.
   */
  virtual void signalRead() { }; // process signals
  virtual void signalClear() { }; // clear signals
};

/**
 * Shows a splash screen while loading resources and then jumps into the
 * Control we supplied.
 */
void runSFML(std::function<std::unique_ptr<Control>()> initCtrl);
}

// elements
#include "elements/button.h"

#endif // SOLEMNSKY_CONTROL_H