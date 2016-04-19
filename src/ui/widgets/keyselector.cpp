#include "keyselector.h"
#include "util/client/sfmlutil.h"

namespace ui {

KeySelector::KeySelector(AppState &appState,
                         const ui::KeySelector::Style &style,
                         const sf::Vector2f &pos) :
    Control(appState),
    capturing(false),
    clickSignal(clickSignal),
    button(appState, style, pos, "") {
  areChildren({&button});
}

void KeySelector::render(ui::Frame &f) {
  Control::render(f);
  const auto body = button.getBody();

  if (capturing) {
    f.drawRect(body, sf::Color::Green);
  }
}

bool KeySelector::handle(const sf::Event &event) {
  if (capturing) {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) setValue({});
      else setValue(event.key.code);

      capturing = false;
      return true;
    }
  }
  return Control::handle(event);
}

void KeySelector::reset() {
  Control::reset();
  capturing = false;
}

void KeySelector::signalRead() {
  if (button.clickSignal) {
    setValue({});
    capturing = true;
  }
  Control::signalRead();
}

void KeySelector::signalClear() {
  Control::signalClear();
}

void KeySelector::setValue(const optional<sf::Keyboard::Key> key) {
  value = key;
  if (key) button.text = printKey(key.get());
  else button.text = "<unbound>";
}

optional<sf::Keyboard::Key> KeySelector::getValue() const {
  return value;
}

void KeySelector::setDescription(const optional<std::string> &description) {
  button.description = description;
}

}

