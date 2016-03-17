#include "button.h"
#include "util/methods.h"
#include "client/util/sfmlutil.h"

namespace ui {

Button::Style::Style(const sf::Color &baseColor,
                     const sf::Color &hotColor,
                     const sf::Color &clickedColor,
                     const sf::Color &inactiveColor,
                     const sf::Color &textColor,
                     const sf::Vector2f &dimensions,
                     const float heatRate,
                     const int fontSize) :
    baseColor(baseColor),
    hotColor(hotColor),
    clickedColor(clickedColor),
    inactiveColor(inactiveColor),
    textColor(textColor),
    dimensions(dimensions),
    heatRate(heatRate),
    fontSize(fontSize) { }

sf::FloatRect Button::getBody() {
  return sf::FloatRect(pos, style.dimensions);
}

Button::Button(const Style &style,
               const sf::Vector2f &pos,
               const std::string &text)
    : pos(pos), text(text), style(style),
      heat(0),
      active(true),
      isHot(false),
      clickSignal(false) { }

void Button::tick(float delta) {
  if (active) heat += (isHot ? 1 : -1) * delta * style.heatRate;
}

void Button::render(Frame &f) {
  const auto body = getBody();
  if (!active) {
    f.drawRect(body, style.inactiveColor);
  } else {
    if (!inPreClick)
      f.drawRect(body, mixColors(style.baseColor, style.hotColor, heat));
    else f.drawRect(body, style.clickedColor);
  }

  const auto size = f.textSize(text, style.fontSize);
  f.drawText(
      sf::Vector2f(
          body.left + (body.width / 2) - (size.x / 2),
          body.top - 5), // TODO: get text bounds to work
      {text}, style.fontSize, style.textColor
  );
}

bool Button::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
    isHot = body.contains(pt);
    return isHot;
  }

  if (event.type == sf::Event::MouseButtonPressed or
      event.type == sf::Event::MouseButtonReleased) {
    bool clicking = event.type == sf::Event::MouseButtonPressed;
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);

    isHot = body.contains(pt);

    if (active) {
      if (clicking) {
        inPreClick = isHot;
      } else {
        inPreClick = false;
        if (isHot) clickSignal = true;
      }
    }
    return isHot;
  }
  return false;
}

void Button::signalClear() {
  clickSignal = false;
}

void Button::setActive(const bool newActive) {
  reset();
  active = newActive;
}

void Button::reset() {
  isHot = false;
  heat = 0;
}

}

