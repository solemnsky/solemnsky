/**
 * A checkbox, especially useful in our settings page. Just a button
 * in a wrapper actually.
 */

#ifndef SOLEMNSKY_CHECKBOX_H
#define SOLEMNSKY_CHECKBOX_H

#include "client/ui/control.h"
#include "button.h"

namespace ui {

class Checkbox: public ui::Control {
 private:
  Button button;
  bool value;

 public:
  typedef Button::Style Style;

  Checkbox() = delete;
  Checkbox(const Style &style,
           const sf::Vector2f &pos);

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

  bool &clickSignal;
};

}

#endif //SOLEMNSKY_CHECKBOX_H
