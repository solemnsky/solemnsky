/**
 * Buttons!
 */
#ifndef SOLEMNSKY_BUTTON_H
#define SOLEMNSKY_BUTTON_H

#include "base/control.h"
#include "base/util.h"

namespace ui {

/**
 * A rectangular button centered on the origin.
 */
class Button : public Control {
private:
public:
  /**
   * Settings
   */
  const struct ButtonSettings {
    sf::Vector2f dimensions; // size of the button
    sf::Color fillColor = sf::Color::Red;

    ButtonSettings(sf::Vector2f dimensions)
        : dimensions(dimensions) { };
  } settings;

  std::string text; // what the button has to say

  Button() = delete;
  Button(sf::Vector2f dimensions);

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
