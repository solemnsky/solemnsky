#include "textentry.h"

namespace ui {

TextEntry::TextEntry(const sf::Vector2f &pos,
                     const std::string &description,
                     const TextEntry::Style &style) :
    pos(pos), description(description), style(style) { }

void TextEntry::tick(float delta) {

}

void TextEntry::render(Frame &f) {
  f.drawRect(pos, pos + sf::Vector2f(style.width, style.height),
             style.bgColor);
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

}

