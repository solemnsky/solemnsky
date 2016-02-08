/**
 * Page where the user receives the comforts of home.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "elements/elements.h"

class HomePage : public Page {
private:
  ui::Button tutorialButton;

public:
  HomePage(ClientShared &state);

  /**
   * Page interface.
   */
  void onLooseFocus() override;
  void onFocus() override;

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
