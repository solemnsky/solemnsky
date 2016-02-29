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
 public:
  struct Style {
    sf::Color baseColor{132, 173, 181},
        hotColor{92, 189, 206},
        clickedColor{63, 208, 234},
        textColor{255, 255, 255};

    sf::Vector2f dimensions{200, 50};

    float heatRate = 10;
    int fontSize = 40;

    Style() { }
  } style;

private:
  Clamped heat;
  bool inPreClick{false};

  sf::FloatRect getBody();

public:
  Button() = delete;
  Button(const sf::Vector2f &pos,
         const std::string &text,
         const Style &style = {});

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
  void reset(); // reset animation

  /**
   * UI state and signals, read-only.
   */
  bool isHot;
  bool clickSignal;
};

}

#endif //SOLEMNSKY_BUTTON_H
