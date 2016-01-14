#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "ui/ui.h"
#include "sky/sky.h"

/**
 * The game client. This is going to get pretty huge fast, our luck is that a
 * lot of the UI can be neatly modularized.
 */
class Client : public ui::Control {
private:
  sky::Sky sky;
public:
  Client();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_CLIENT_H
