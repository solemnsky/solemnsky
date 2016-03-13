/**
 * Buttons!
 */
#ifndef SOLEMNSKY_BUTTON_H
#define SOLEMNSKY_BUTTON_H

#include "client/ui/control.h"
#include "util/types.h"

namespace ui {

/**
 * A rectangular button.
 */
class Button: public Control {
 public:
  struct Style {
    sf::Color baseColor, hotColor, clickedColor, textColor;
    sf::Vector2f dimensions;

    float heatRate;
    int fontSize;

    Style() = delete;
    Style(sf::Color baseColor,
          sf::Color hotColor,
          sf::Color clickedColor,
          sf::Color textColor,
          sf::Vector2f dimensions,
          float heatRate,
          int fontSize);
  } style;

 private:
  Clamped heat;
  bool inPreClick{false};
  bool active;

  sf::FloatRect getBody();

 public:
  Button() = delete;
  Button(const sf::Vector2f &pos,
         const std::string &text,
         const Style &style);

  sf::Vector2f pos;
  std::string text; // what the button has to say

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalClear() override;

  /**
   * UI methods.
   */
  void setActive(const bool active);
  void reset(); // reset animation

  /**
   * UI state and signals, read-only.
   */
  bool isHot;
  bool clickSignal;
};

}

#endif //SOLEMNSKY_BUTTON_H
