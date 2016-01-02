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

  sf::FloatRect getBody();

public:
  /**
   * Settings
   */
  struct Style {
    sf::Color color{sf::Color::Green}, hotColor{sf::Color::Red};
    float width{200}, height{100};

    float hotAnimLength = 0.5;
    int fontSize = 24;

    Style() { }

  } style;

  sf::Vector2f pos;
  std::string text; // what the button has to say

  Button() = delete;
  Button(const sf::Vector2f pos, const std::string text,
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
