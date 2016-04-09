/**
 * The ubiquitous text entry.
 */
#pragma once
#include "util/types.h"
#include "ui/control.h"

namespace ui {

class TextEntry: public Control {
 public:
  struct Style {
    sf::Color inactiveColor{142, 183, 191}, // background when inactive
        hotColor{162, 203, 211}, // background when active (hot)
        focusedColor{255, 255, 255},
        descriptionColor{100, 100, 100}, // description text
        textColor{0, 0, 0}; // text
    sf::Vector2f dimensions{500, 40};
    int fontSize = 30;
    float heatRate = 10;

    Style() = delete;
    Style(const sf::Color &inactiveColor,
          const sf::Color &hotColor,
          const sf::Color &focusedColor,
          const sf::Color &descriptionColor,
          const sf::Color &textColor,
          const sf::Vector2f &dimensions,
          const int fontSize,
          const float heatRate);
  } style;

 private:
  // magic values
  const float cursorWidth = 5, sidePadding = 10;

  Clamped heat;

  float scroll;
  int cursor;

  optional<sf::Event> pressedKeyboardEvent;
  Cooldown repeatActivate;
  Cooldown repeatCooldown;

  TextFormat textFormat, descriptionFormat; // deduced from the style

  sf::FloatRect getBody();
  void handleKeyboardEvent(const sf::Event &event);

 public:
  TextEntry() = delete;
  TextEntry(const Style &style,
            const sf::Vector2f &pos,
            const std::string &description = "",
            const bool persistent = false);

  bool persistent; // text is persistent, doesn't reset on focus change / entry
  // and the displayed description is the contents
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
