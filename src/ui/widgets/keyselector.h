/**
 * Widget used to select a keyboard key.
 */
#pragma once
#include "ui/control.h"
#include "button.h"

namespace ui {

class KeySelector: public ui::Control {
 private:
  Button button;
  optional<sf::Keyboard::Key> value;
  bool capturing;
  std::string printKey(const sf::Keyboard::Key key) const;

 public:
  typedef Button::Style Style;

  KeySelector(AppState &appState,
              const Style &style, const sf::Vector2f &pos);

  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  /**
   * Custom API.
   */
  void setValue(const optional<sf::Keyboard::Key> key);
  optional<sf::Keyboard::Key> getValue() const;

  void setDescription(const optional<std::string> &description);

  bool &clickSignal;
};

}
