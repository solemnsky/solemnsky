/**
 * Page where the user recieves the comforts of home.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "ui/ui.h"
#include "clientstate.h"
#include "page.h"

class HomePage : public Page {
private:
  ui::Button tutorialButton;
  ClientState *const state;

public:
  HomePage(ClientState *const state);

  /**
   * Page interface.
   */
  void reset() override;

  /**
   * Control interface.
   */
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
