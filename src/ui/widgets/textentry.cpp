#include "textentry.h"
#include "client/util/sfmlutil.h"
#include "util/methods.h"

namespace ui {

ui::TextEntry::Style::Style(
    const sf::Color &inactiveColor,
    const sf::Color &hotColor,
    const sf::Color &focusedColor,
    const sf::Color &descriptionColor,
    const sf::Color &textColor,
    const sf::Vector2f &dimensions,
    const int fontSize,
    const float heatRate) :
    inactiveColor(inactiveColor),
    hotColor(hotColor),
    focusedColor(focusedColor),
    descriptionColor(descriptionColor),
    textColor(textColor),
    dimensions(dimensions),
    fontSize(fontSize),
    heatRate(heatRate) { }

TextEntry::TextEntry(const Style &style,
                     const sf::Vector2f &pos,
                     const std::string &description,
                     const bool persistent) :
    persistent(persistent),
    pos(pos),
    scroll(0),
    cursor(0),
    heat(0),
    description(description),
    style(style),

    pressedKeyboardEvent(),
    repeatActivate(0.3),
    repeatCooldown(0.06), // TODO: Cooldowns don't compensate for
    // wrap-around, making their behaviour erratic when the tick interval
    // approaches the cooldown interval

    isHot(false),
    isFocused(false),

    textFormat(style.fontSize, {},
               ui::HorizontalAlign::Left, ui::VerticalAlign::Top,
               ResID::Font),
    descriptionFormat(textFormat) {
  descriptionFormat.horizontal = HorizontalAlign::Right;
  descriptionFormat.vertical = VerticalAlign::Middle;
}

sf::FloatRect TextEntry::getBody() {
  return sf::FloatRect(pos, style.dimensions);
}

bool TextEntry::handleKeyboardEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    sf::Keyboard::Key key = event.key.code;

    switch (key) {
      case sf::Keyboard::Escape: {
        unfocus();
        return true;
      }
      case sf::Keyboard::Return: {
        inputSignal.emplace(contents);
        unfocus();
        return true;
      }
      case sf::Keyboard::BackSpace: {
        if (!contents.empty() and cursor != 0) {
          contents.erase((size_t) cursor - 1, 1);
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
        break;
    }
  }

  if (event.type == sf::Event::TextEntered) {
    if (event.text.unicode < 32) return true; // non-printable
    contents.insert((size_t) cursor, {(char) event.text.unicode});
    cursor++;
    return true;
  }

  return false;
}

void TextEntry::tick(float delta) {
  heat += (isHot ? 1 : -1) * delta * style.heatRate;

  if (pressedKeyboardEvent) {
    if (repeatActivate.cool(delta)) {
      if (repeatCooldown.cool(delta)) {
        handleKeyboardEvent(*pressedKeyboardEvent);
        repeatCooldown.reset();
      }
    }
  }
}

void TextEntry::render(Frame &f) {
  f.pushTransform(sf::Transform().translate(pos));
  if (persistent) {
    f.drawText({-20, style.dimensions.y / 2.0f},
               description, style.textColor, descriptionFormat);
  }

  if (isFocused) {
    f.drawRect({}, style.dimensions, style.focusedColor);
    f.drawText(
        sf::Vector2f(sidePadding - scroll, 0), [&](TextFrame &tf) {
          tf.print(contents.substr(0, size_t(cursor)));
          const float scroll =
              (tf.drawOffset.x > (style.dimensions.x + sidePadding))
              ? tf.drawOffset.x - style.dimensions.x + 2 * sidePadding : 0;
          f.drawRect(
              {sidePadding + tf.drawOffset.x - scroll, 0},
              {sidePadding + tf.drawOffset.x - scroll +
                  cursorWidth, style.dimensions.y},
              style.textColor);
          tf.print(contents.substr(size_t(cursor)));
        }, style.textColor, textFormat);
  } else {
    f.drawRect({}, style.dimensions,
               mixColors(style.inactiveColor, style.hotColor, heat));
    f.drawText({sidePadding, 0},
               {persistent ? contents : description},
               style.textColor, textFormat);
  }
  f.popTransform();
}

bool TextEntry::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseButtonPressed or
      event.type == sf::Event::MouseButtonReleased) {
    bool clicked = event.type == sf::Event::MouseButtonPressed;
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);

    bool contains = body.contains(pt);
    if (clicked) {
      if (contains) focus();
      else unfocus();
    }

    return contains;
  }
  if (!isFocused) {
    if (event.type == sf::Event::MouseMoved) {
      const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
      isHot = body.contains(pt);
      return isHot;
    }
  } else {
    if (event.type == sf::Event::KeyReleased) {
      pressedKeyboardEvent.reset();
      repeatActivate.reset();
      repeatCooldown.reset();
      return false;
    }

    if (event.type == sf::Event::KeyPressed) {
      // don't repeat enter key or escape key
      if (event.key.code != sf::Keyboard::Return
          and event.key.code != sf::Keyboard::Escape)
        pressedKeyboardEvent = event;

      if (handleKeyboardEvent(event)) return true;
    }

    if (event.type == sf::Event::TextEntered) {
      if (handleKeyboardEvent(event)) return true;
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
  if (persistent) cursor = (int) contents.size();
}

void TextEntry::unfocus() {
  isFocused = false;
  if (!persistent) {
    contents = "";
    cursor = 0;
  }
  repeatActivate.reset();
  repeatCooldown.reset();
}

}

