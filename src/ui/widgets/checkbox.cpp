#include "checkbox.h"
#include "util/methods.h"

namespace ui {

Checkbox::Checkbox(AppState &appState,
                   const ui::Button::Style &style,
                   const sf::Vector2f &pos) :
    Control(appState),
    button(appState, style, pos, ""),
    value(false),
    clickSignal(button.clickSignal) {
  areChildren({&button});
}

void Checkbox::signalRead() {
  if (button.clickSignal) {
    setValue(!value);
  }
}

void Checkbox::signalClear() {
  button.signalClear();
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
