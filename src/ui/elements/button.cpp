#include "button.h"

namespace ui {

Button::Button(const sf::FloatRect body, Button::Style style) :
    body(body), style(style) { }

void Button::tick(float delta) {
}

void Button::render(Frame &f) {
  f.drawRect(body);
}

void Button::handle(sf::Event event) {
  if (Optional<bool> buttonPressed = getMouseButtonAction(event)) {
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);
    isHot = body.contains(pt);
    isClicked = isHot && *buttonPressed;
  }
}

}

