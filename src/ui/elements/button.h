/**
 * Buttons!
 */
#ifndef SOLEMNSKY_BUTTON_H
#define SOLEMNSKY_BUTTON_H

#include "ui/control.h"
#include "base/base.h"
#include  <SFML/Graphics/Color.hpp>
#include <ui/signal.h>

namespace ui {

/**
 * A rectangular button.
 */
class Button : public Control {
private:
  Clamped hotAnimState;
  bool inPreClick{false};

  inline float getHeat() { return hotAnimState / style.hotAnimLength; }

  sf::FloatRect getBody();

public:
  /**
   * Settings
   */
  struct Style {
    sf::Color color{132, 173, 181},
        hotColor{92, 189, 206},
        clickedColor{63, 208, 234},
        textColor{255, 255, 255};

    float width{200}, height{100};

    float hotAnimLength = 0.1;
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
  void tick(float delta) override;
  void render(Frame &f) override;
  void handle(sf::Event event) override;
  void signalClear() override;

  /**
   * Signal.
   */
  bool isHot{false}; // if the mouse is hovering over it
  ui::Signal<ui::None> clickSignal;
};

}

#endif //SOLEMNSKY_BUTTON_H
