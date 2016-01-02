#include "button.h"

namespace ui {

Button::Button(const sf::FloatRect body, const std::string text,
               Button::Style style) :
    body(body), text(text), style(style) { }

void Button::tick(float delta) {
  if (isHot) hotAnimState += delta;
  else hotAnimState -= delta;
  hotAnimState = clamp(0.0f, style.hotAnimLength, hotAnimState);
}

void Button::render(Frame &f) {
  f.drawRect(body, mixColors(style.color, style.hotColor, getHeat()));
  f.drawText(sf::Vector2f(0, 0), text);
}

void Button::handle(sf::Event event) {
  if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
    isHot = body.contains(pt);
  }
  if (Optional<bool> buttonPressed = getMouseButtonAction(event)) {
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);
    isHot = body.contains(pt);
    isClicked = isHot && *buttonPressed;
  }
}

}

