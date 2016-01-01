#include "button.h"

namespace ui {


Button::Button(sf::Vector2f dimensions) :
    settings(dimensions) {

}

void Button::tick(float delta) {

}

void Button::render(Frame &f) {
  f.drawRect(settings.dimensions);
}

void Button::handle(sf::Event event) {
  if (Optional<bool> buttonPressed = getMouseButtonAction(event)) {
    const int x(event.mouseButton.x), y(event.mouseButton.y);
    isHot = settings.dimensions.x > std::abs(x) &&
            settings.dimensions.y > std::abs(y);
    isClicked = isHot && *buttonPressed;
  }
}

}

