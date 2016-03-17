/**
 * Page where the user receives the comforts of home.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "elements/elements.h"

class HomePage: public Page {
 private:
  ui::Button tutorialButton;
  ui::Button localhostButton;

 public:
  HomePage(ClientShared &state);

  /**
   * Page interface.
   */
  void onChangeSettings(const SettingsDelta &settings) override;
  void onBlur() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
