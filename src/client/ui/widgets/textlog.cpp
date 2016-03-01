#include "textlog.h"

namespace ui {

/**
 * TextLogEntry.
 */

namespace detail {

TextLogEntry::TextLogEntry(std::string &&text, const double time) :
    text(std::move(text)), time(time) { }

}

/**
 * TextLog.
 */

TextLog::TextLog(const sf::Vector2f &pos, const Style &style) :
    expanded(true),
    lifetime(0),
    pos(pos),
    style(style) { }

void TextLog::tick(float delta) {
  lifetime += delta;
}

void TextLog::render(Frame &f) {
  if (expanded) {
    float yOffset = 0;
    for (size_t i = entries.size() - 1; entries.size() > i; --i) {
      f.drawText(pos - sf::Vector2f(0, yOffset + style.fontSize),
                 {entries[i].text},
                 style.fontSize, style.textColor);
      yOffset += (float) style.fontSize;
    }
  }
}

bool TextLog::handle(const sf::Event &event) {
  return false;
}

void TextLog::pushEntry(const std::string &text) {
  if (entries.size() == style.maxEntries) entries.erase(entries.begin());
  entries.push_back(detail::TextLogEntry(std::move(text), lifetime));
}

}
