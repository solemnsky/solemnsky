/**
 * Page where the user recieves the comforts of home.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "ui/ui.h"
#include "clientstate.h"

class HomePage : public ui::Control {
private:
  ClientState *state;

public:
  HomePage(ClientState *state);

  /**
   * Control interface.
   */
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
