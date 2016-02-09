/**
 * Generic, highly customizable text entry, with cursor and mouse interaction
 * and fancy stuff like that.
 */
#ifndef SOLEMNSKY_TEXTENTRY_H
#define SOLEMNSKY_TEXTENTRY_H

#include "util/types.h"
#include "client/ui/control.h"

namespace ui {

class TextEntry : public Control {
private:
  Clamped heat;

  float scroll;
  int cursor;
  sf::FloatRect getBody();

public:
  /**
   * Style settings.
   */
  struct Style {
    sf::Color inactiveBgColor{142, 183, 191}, // background when inactive
        hotBgColor{162, 203, 211}, // background when active (hot)
        focusedBgColor{255, 255, 255},
        descriptionColor{100, 100, 100}, // description text
        textColor{0, 0, 0}; // text
    sf::Vector2f dimensions{500, 40};
    float cursorWidth = 5;
    float sidePadding = 10;
    int fontSize = 30;
    float heatRate = 10;

    Style() { }
  } style;

  TextEntry() = delete;
  TextEntry(const sf::Vector2f &pos,
            const std::string &description = "",
            const Style &style = {});

  sf::Vector2f pos;

  /**
   * Control implementation.
   */
  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
  virtual void signalRead() override;
  virtual void signalClear() override;

  /**
   * UI methods.
   */
  std::string contents;
  std::string description;
  void reset(); // reset animations
  void focus();
  void unfocus();

  /**
   * UI state and signals, read-only.
   */
  bool isHot;
  bool isFocused;
  optional<std::string> inputSignal;
};

}

#endif //SOLEMNSKY_TEXTENTRY_H
