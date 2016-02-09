#include "button.h"
#include "client/util/sfmlutil.h"

namespace ui {

sf::FloatRect Button::getBody() {
  return sf::FloatRect(pos, style.dimensions);
}

Button::Button(const sf::Vector2f &pos, const std::string &text,
               const Style &style) :
    pos(pos), text(text), style(style),
    heat(0, 1, 0) { }

void Button::tick(float delta) {
  heat += (isHot ? 1 : -1) * delta * style.heatRate;
}

void Button::render(Frame &f) {
  f.pushAlpha(style.alpha);
  const auto body = getBody();
  if (!inPreClick)
    f.drawRect(body, mixColors(style.baseColor, style.hotColor, heat));
  else f.drawRect(body, style.clickedColor);
  const auto size = f.textSize(text, style.fontSize);
  f.drawText(
      sf::Vector2f(
          body.left + (body.width / 2),
          body.top + (body.height / 2)) - (size * 0.5f),
      {text}, style.fontSize, style.textColor
  );
  f.popAlpha();
}

bool Button::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
    isHot = body.contains(pt);
    return true;
  }
  if (event.type == sf::Event::MouseButtonPressed or
      event.type == sf::Event::MouseButtonReleased) {
    bool clicking = event.type == sf::Event::MouseButtonPressed;
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);

    isHot = body.contains(pt);

    if (clicking) {
      inPreClick = isHot;
    } else {
      inPreClick = false;
      if (isHot) clickSignal = true;
    }
    return isHot;
  }
  return false;
}

void Button::signalClear() {
  clickSignal = false;
}

void Button::reset() {
  isHot = false;
}
}

