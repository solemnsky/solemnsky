/**
 * The concept of a Control object is the basis of our UI abstraction.
 * Nothing interesting or tricky here, just an interface that supports ticking
 * with time, rendering to a ui::Frame, and handling user input events from 
 * SFML.
 */
#ifndef SOLEMNSKY_CONTROL_H
#define SOLEMNSKY_CONTROL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <functional>
#include <memory>
#include "frame.h"

namespace ui {

/**
 * A user interface thingy.
 */
class Control {
public:
  /**
   * Signals to the app loop.
   */
  std::shared_ptr<Control> next{nullptr};
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
 * Finally, we can turn all this into an application with runSFML, using the
 * SFML 2 lib.
 */
void runSFML(std::shared_ptr<Control> ctrl);
}

#endif // SOLEMNSKY_CONTROL_H
