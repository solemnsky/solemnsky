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
public:
  /**
   * Style settings.
   */
  struct Style {
    sf::Color bgColor{255, 255, 255}, // background
      descriptionColor{0, 0, 0}, // description text, when empty and unfocused
      textColor{0, 0, 0}; // text, when not empty
    float width{500}, height{50};
    float fontSize = 40;

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
  bool focused;
};

}

#endif //SOLEMNSKY_TEXTENTRY_H
