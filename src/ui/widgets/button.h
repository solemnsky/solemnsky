/**
 * The ubiquitous button.
 */
#pragma once
#include "ui/control.h"
#include "util/types.h"

namespace ui {

/**
 * A rectangular button.
 */
class Button: public Control {
 public:
  struct Style {
    sf::Color baseColor, hotColor, clickedColor, inactiveColor, textColor;
    sf::Vector2f dimensions;
    float heatRate;
    int fontSize;

    Style() = delete;
    Style(const sf::Color &baseColor,
          const sf::Color &hotColor,
          const sf::Color &clickedColor,
          const sf::Color &inactiveColor,
          const sf::Color &textColor,
          const sf::Vector2f &dimensions,
          const float heatRate,
          const int fontSize);
  } style;

 private:
  Clamped heat;
  bool inPreClick{false};
  bool active;

  sf::FloatRect getBody();

  TextFormat textFormat; // deduced from the style

 public:
  Button() = delete;
  Button(const Style &style,
         const sf::Vector2f &pos,
         const std::string &text);

  sf::Vector2f pos;
  std::string text; // what the button has to say

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalClear() override;
  void reset() override;

  /**
   * Custom.
   */
  void setActive(const bool active);

  bool isHot;
  bool clickSignal;
};

}
