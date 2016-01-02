/**
 * Buttons!
 */
#ifndef SOLEMNSKY_BUTTON_H
#define SOLEMNSKY_BUTTON_H

#include "ui/control.h"
#include "base/util.h"

namespace ui {

/**
 * A rectangular button.
 */
class Button : public Control {
private:
public:
  /**
   * Settings
   */
  struct Style {
    Style() { }
  } style;

  sf::FloatRect body;
  std::string text; // what the button has to say

  Button() = delete;
  Button(const sf::FloatRect body, Style style = {});

  /**
   * Control implementation.
   */
  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual void handle(sf::Event event) override;

  /**
   * UI status.
   */
  bool isClicked{false};
  bool isHot{false}; // if the mouse is hovering over it
};

}

#endif //SOLEMNSKY_BUTTON_H
