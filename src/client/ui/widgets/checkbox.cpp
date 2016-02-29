#include "checkbox.h"

/**
 * Checkbox.
 */

namespace ui {

Checkbox::Style::Style() {
  baseButtonStyle.dimensions = {40, 40};
}

Button::Style Checkbox::makeButtonStyle() const {
  Button::Style bstyle(style.baseButtonStyle);
  bstyle.dimensions = {style.dimensions, style.dimensions};
  return bstyle;
}

Checkbox::Checkbox(const sf::Vector2f &pos,
                   const ui::Checkbox::Style &style) :
    style(style),
    button(pos, "", makeButtonStyle()) { }

void Checkbox::tick(float delta) {
  button.tick(delta);
}

void Checkbox::render(Frame &f) {
  button.render(f);
}

bool Checkbox::handle(const sf::Event &event) {
  return button.handle(event);
}

void Checkbox::signalRead() {
  if (button.clickSignal) {
    clickSignal = true;
    setValue(!value);
  }
}

void Checkbox::signalClear() {
  button.signalClear();
  clickSignal = false;
}

void Checkbox::reset() {
  button.reset();
}

void Checkbox::setValue(const bool value) {
  this->value = value;
  button.text = value ? "x" : " ";
}

bool Checkbox::getValue() const {
  return value;
}

}
