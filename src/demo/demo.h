/**
 * A cute demo for things.
 */
#ifndef SOLEMNSKY_DEMO_H
#define SOLEMNSKY_DEMO_H

#include "base/control.h"
#include "base/ui.h"

class Demo : public Control {
private:
  ui::Button button{{20, 20}};

public:
  void tick(float delta) override;
  void render(Frame &f) override;
  void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_DEMO_H
