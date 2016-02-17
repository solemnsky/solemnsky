/**
 * Page where the user receives the comforts of home.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "elements/elements.h"

class HomePage: public Page {
 private:
  struct Style {
    sf::Vector2f tutorialButtonPos{500, 300},
        localhostButtonPos{1100, 300};

    std::string tutorialButtonDesc = "start tutorial",
        localhostButtonDesc = "connect to localhost";

    Style() { }
  } style;

  ui::Button tutorialButton;
  ui::Button localhostButton;

 public:
  HomePage(ClientShared &state);

  /**
   * Page interface.
   */
  void onLooseFocus() override;
  void onFocus() override;
  void onChangeSettings(const SettingsDelta &settings) override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
