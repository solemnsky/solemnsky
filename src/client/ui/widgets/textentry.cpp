#include "textentry.h"
#include "client/util/sfmlutil.h"
#include "util/methods.h"

namespace ui {

TextEntry::TextEntry(const sf::Vector2f &pos,
                     const std::string &description,
                     const TextEntry::Style &style) :
    pos(pos),
    cursor(0),
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
    f.pushTransform(sf::Transform().translate(pos));

    const sf::Vector2f textDims(
        f.textSize(contents.substr(0, (size_t) cursor), style.fontSize));
    const float scroll =
        (textDims.x > (style.dimensions.x + style.sidePadding))
        ? textDims.x - style.dimensions.x + 2 * style.sidePadding : 0;

    f.drawRect({}, style.dimensions, style.focusedBgColor);
    f.drawText(sf::Vector2f(style.sidePadding - scroll, 0),
               {contents}, style.fontSize, style.textColor);

    f.drawRect(
        {style.sidePadding + textDims.x - scroll, 0},
        {style.sidePadding + textDims.x +
         style.cursorWidth, style.dimensions.y},
        style.textColor);

    f.popTransform();
  } else {
    f.drawRect(pos, pos + style.dimensions,
               mixColors(style.inactiveBgColor, style.hotBgColor, heat));
    f.drawText(pos + sf::Vector2f(style.sidePadding, 0),
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
      switch (event.key.code) {
        case sf::Keyboard::Escape: {
          isFocused = false;
          contents.clear();
          return true;
        }
        case sf::Keyboard::Return: {
          isFocused = false;
          inputSignal.emplace(std::move(contents));
          return true;
        }
        case sf::Keyboard::BackSpace: {
          if (!contents.empty() and cursor != 0) {
            contents.erase((size_t) cursor - 1, 1);
            appLog(LogType::Debug, contents);
            cursor--;
          }
          return true;
        }
        case sf::Keyboard::Left: {
          cursor = std::max(0, cursor - 1);
          return true;
        }
        case sf::Keyboard::Right: {
          cursor = std::min((int) contents.size(), cursor + 1);
          return true;
        }
        default:
          return false;
      }
    }

    if (event.type == sf::Event::TextEntered) {
      if (event.text.unicode == 8) return true; // backspace character
      contents.insert((size_t) cursor, {(char) event.text.unicode});
      cursor++;
      return true;
    }
  }
  return false;
}

void TextEntry::signalRead() { }

void TextEntry::signalClear() {
  inputSignal.reset();
}

void TextEntry::reset() {
  isHot = false;
}

void TextEntry::focus() {
  isFocused = true;
}

void TextEntry::unFocus() { }

}

