/**
 * A homepage for our client, with all the comforts of home. This is where
 * the user enters and can come back to when he exits something.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "ui/ui.h"

class HomePage : public ui::Control {
private:
public:
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
