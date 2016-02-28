/**
 * A checkbox with a description, useful in our settings page.
 */

#ifndef SOLEMNSKY_CHECKBOX_H
#define SOLEMNSKY_CHECKBOX_H

#include "client/ui/control.h"

namespace ui {

class RadioButton: public ui::Control {
 public:
  struct Style {
    sf::Color baseColor{132, 173, 181},
        hotColor{92, 189, 206},
        clickedColor{63, 208, 234},
        fontColor{255, 255, 255};
    float checkboxSize = 40; // side length
    float heatRate = 10;

    Style() { }
  } style;
 private:
  Clamped heat;
  bool clicked;

 public:
  RadioButton() = delete;
  RadioButton(const sf::Vector2f &pos,
              const Style &style = {});

  sf::Vector2f pos;

  virtual void tick(float delta);
  virtual void render(Frame &f);
  virtual bool handle(const sf::Event &event);

  /**
   * UI state and signals, read-only.
   */
  bool isHot{false};
  bool clickSignal{false};
};

}

#endif //SOLEMNSKY_CHECKBOX_H
