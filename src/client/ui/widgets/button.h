/**
 * The ubiquitous button.
 */
#pragma once
#include "client/ui/control.h"
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
    TextProperties textProp;

    Style() = delete;
    Style(const sf::Color &baseColor,
          const sf::Color &hotColor,
          const sf::Color &clickedColor,
          const sf::Color &inactiveColor,
          const sf::Color &textColor,
          const sf::Vector2f &dimensions,
          const float heatRate,
          const TextProperties &textProp);
  } style;

 private:
  Clamped heat;
  bool inPreClick{false};
  bool active;

  sf::FloatRect getBody();

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
