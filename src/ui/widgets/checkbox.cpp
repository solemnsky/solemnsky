#include "checkbox.h"
#include "util/methods.h"

namespace ui {

Checkbox::Checkbox(const ui::Button::Style &style,
                   const sf::Vector2f &pos) :
    button(style, pos, ""),
    value(false),
    clickSignal(button.clickSignal) { }

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
    setValue(!value);
  }
}

void Checkbox::signalClear() {
  button.signalClear();
}

void Checkbox::reset() {
  button.reset();
}

void Checkbox::setValue(const bool newValue) {
  value = newValue;
  button.text = newValue ? "x" : " ";
}

bool Checkbox::getValue() const {
  return value;
}

void Checkbox::setDescription(const optional<std::string> &description) {
  button.description = description;
}

}
