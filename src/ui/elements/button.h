/**
 * Buttons!
 */
#ifndef SOLEMNSKY_BUTTON_H
#define SOLEMNSKY_BUTTON_H

#include "ui/control.h"
#include "base/util.h"
#include  <SFML/Graphics/Color.hpp>

namespace ui {

/**
 * A rectangular button.
 */
class Button : public Control {
private:
  float hotAnimState = 0;

  inline float getHeat() { return hotAnimState / style.hotAnimLength; }

public:
  /**
   * Settings
   */
  struct Style {
    sf::Color color{sf::Color::Green}, hotColor{sf::Color::Red};
    float hotAnimLength = 0.5;

    Style() { }
  } style;

  sf::FloatRect body;
  std::string text; // what the button has to say

  Button() = delete;
  Button(const sf::FloatRect body, const std::string text,
         Button::Style style = {});

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
