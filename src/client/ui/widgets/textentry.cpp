#include "textentry.h"
#include "client/util/sfmlutil.h"

namespace ui {

TextEntry::TextEntry(const sf::Vector2f &pos,
                     const std::string &description,
                     const TextEntry::Style &style) :
    pos(pos),
    heat(0, 1, 0),
    description(description),
    style(style),

    isHot(false),
    isFocused(false) { }

sf::FloatRect TextEntry::getBody() {
  return sf::FloatRect(pos, style.dimensions);
}

void TextEntry::tick(float delta) {
  heat += (isHot ? 1 : -1) * delta * style.heatRate;
}

void TextEntry::render(Frame &f) {
  if (isFocused) {
    f.drawRect(pos, pos + style.dimensions, style.focusedBgColor);
    f.drawText(pos + sf::Vector2f(style.leftPadding, 0),
               {contents}, style.fontSize, style.textColor);
  } else {
    f.drawRect(pos, pos + style.dimensions,
               mixColors(style.inactiveBgColor, style.hotBgColor, heat));
    f.drawText(pos + sf::Vector2f(style.leftPadding, 0),
               {description}, style.fontSize, style.descriptionColor);
  }
}

bool TextEntry::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseButtonPressed or
      event.type == sf::Event::MouseButtonReleased) {
    bool clicked = event.type == sf::Event::MouseButtonPressed;
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);

    bool contains = body.contains(pt);
    if (clicked) isFocused = contains;
    return contains;
  }
  if (!isFocused) {
    if (event.type == sf::Event::MouseMoved) {
      const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
      isHot = body.contains(pt);
      return isHot;
    }
  } else {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) {
        isFocused = false;
        contents.clear();
        return true;
      }
      if (event.key.code == sf::Keyboard::Return) {
        isFocused = false;
        inputSignal.emplace(std::move(contents));
        return true;
      }
      if (event.key.code == sf::Keyboard::BackSpace) {
        contents.pop_back();
        return true;
      }
    }

    if (event.type == sf::Event::TextEntered) {
      contents += event.text.unicode;
      return true;
    }
  }
  return false;
}

void TextEntry::signalRead() { }

void TextEntry::signalClear() {
  inputSignal.reset();
}

void TextEntry::focus() {

}

void TextEntry::unFocus() {

}

}

