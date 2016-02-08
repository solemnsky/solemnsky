#include "textentry.h"

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

}

void TextEntry::render(Frame &f) {
  f.drawRect(pos, pos + style.dimensions,
             style.inactiveBgColor);
  f.drawText(pos - sf::Vector2f(0, style.fontSize),
             {description}, style.fontSize, style.descriptionColor);
}

bool TextEntry::handle(const sf::Event &event) {
  return false;
}

void TextEntry::signalRead() {
  Control::signalRead();
}

void TextEntry::signalClear() {
  Control::signalClear();
}

void TextEntry::focus() {

}

void TextEntry::unFocus() {

}

}

