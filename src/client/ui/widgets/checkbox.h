/**
 * A checkbox with a description, useful in our settings page. Just a button
 * in a wrapper.
 */

#ifndef SOLEMNSKY_CHECKBOX_H
#define SOLEMNSKY_CHECKBOX_H

#include "client/ui/control.h"
#include "button.h"

namespace ui {

class Checkbox: public ui::Control {
 public:
  struct Style {
    Button::Style baseButtonStyle;
    float dimensions = 40; // we make the button a square with these dimensions
    Style();
  } style;
 private:
  Button::Style makeButtonStyle() const;
  Button button;

  bool value;

 public:
  Checkbox() = delete;
  Checkbox(const sf::Vector2f &pos,
           const Style &style = {});

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  /**
   * UI methods.
   */
  void reset();

  /**
   * Signals and UI values.
   */
  void setValue(const bool newValue);
  bool getValue() const;

  bool clickSignal;
};

}

#endif //SOLEMNSKY_CHECKBOX_H
