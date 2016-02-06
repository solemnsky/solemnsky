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
class Button : public Control {
private:
  Clamped hotAnimState;
  bool inPreClick{false};

  inline float getHeat() { return hotAnimState / style.hotAnimLength; }

  sf::FloatRect getBody();

public:
  /**
   * Style settings.
   */
  const struct Style {
    sf::Color baseColor{132, 173, 181},
        hotColor{92, 189, 206},
        clickedColor{63, 208, 234},
        textColor{255, 255, 255};

    float width{200}, height{100};

    float hotAnimLength = 0.1;
    int fontSize = 40;
    float alpha = 1;

    Style() = default;
  } style;

  Button() = delete;
  Button(const sf::Vector2f &pos,
         const std::string &text,
         const Style &style = {});

  sf::Vector2f pos;
  std::string text; // what the button has to say

  void reset(); // reset animation, pretend it was recreated

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalClear() override;

  /**
   * UI state and signals.
   */
  bool isHot{false}; // if the mouse is hovering over it
  ui::Signal<ui::None> clickSignal;
};

}

#endif //SOLEMNSKY_BUTTON_H
