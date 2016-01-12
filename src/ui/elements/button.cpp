#include "button.h"

namespace ui {

sf::FloatRect Button::getBody() {
  float w(style.width), h(style.height);
  return {pos.x - (w / 2), pos.y - (h / 2), w, h};
}

Button::Button(const sf::Vector2f pos, const std::string text,
               Button::Style style) :
    pos(pos), text(text), style(style),
    hotAnimState(0, style.hotAnimLength, 0) { }

void Button::tick(float delta) {
  if (isHot) hotAnimState += delta;
  else hotAnimState -= delta;
}

void Button::render(Frame &f) {
  f.pushAlpha(style.alpha);
  const auto body = getBody();
  if (!inPreClick)
    f.drawRect(body, mixColors(style.color, style.hotColor,
                               getHeat()));
  else f.drawRect(body, style.clickedColor);
  const auto size = f.textSize(text, 24);
  f.drawText(
      sf::Vector2f(
          body.left + (body.width / 2),
          body.top + (body.height / 2)) - (size * 0.5f),
      {text}, style.fontSize, style.textColor
  );
  f.popAlpha();
}

void Button::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
    isHot = body.contains(pt);
  }
  if (optional<bool> buttonPressed = getMouseButtonAction(event)) {
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);
    isHot = body.contains(pt);

    if (*buttonPressed) {
      inPreClick = isHot;
    } else {
      inPreClick = false;
      if (isHot) clickSignal += {};
    }
  }
}

void Button::signalClear() {
  clickSignal.clear();
}
}

