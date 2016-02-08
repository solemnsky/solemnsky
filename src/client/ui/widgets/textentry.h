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

  sf::FloatRect getBody();

public:
  /**
   * Style settings.
   */
  struct Style {
    sf::Color inactiveBgColor{142, 183, 191}, // background when inactive
        hotBgColor{162, 203, 211}, // background when active (hot)
        focusedBgColor{255, 255, 255},
        descriptionColor{20, 20, 20}, // description text
        textColor{0, 0, 0}; // text
    sf::Vector2f dimensions{500, 50};
    int fontSize = 40;

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
  void focus();
  void unFocus();

  /**
   * UI state and signals, read-only.
   */
  bool isHot;
  bool isFocused;
};

}

#endif //SOLEMNSKY_TEXTENTRY_H
