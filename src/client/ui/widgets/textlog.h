/**
 * Vertical log of text entries that occured. Has neat animations.
 */
#ifndef SOLEMNSKY_TEXTLOG_H
#define SOLEMNSKY_TEXTLOG_H

#include "client/ui/control.h"

namespace ui {

namespace detail {
struct TextLogEntry {
  TextLogEntry(std::string &&text, const double time);

  std::string text;
  double time;
};

}

class TextLog : public Control {
private:
  double lifetime;

public:
  /**
   * Style settings.
   */
  struct Style {
    sf::Color textColor{255, 255, 255};
    float sidePadding;
    float maxWidth = 500;
    int fontSize = 30;

    float entryLifetime = 10;
    // for how long in seconds a log is displayed in unexpanded mode

    Style() { }
  } style;

  TextLog() = delete;
  TextLog(const sf::Vector2f &pos,
          const Style &style = {});

  sf::Vector2f pos; // bottom left, text prints upwards
  std::vector<detail::TextLogEntry> entries;

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;

  /**
   * UI methods.
   */
  bool expanded; // set this
  void pushEntry(std::string &&text);
};

}

#endif //SOLEMNSKY_TEXTLOG_H
