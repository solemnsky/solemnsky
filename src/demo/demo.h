/**
 * A cute demo for things.
 */
#ifndef SOLEMNSKY_DEMO_H
#define SOLEMNSKY_DEMO_H

#include "ui/ui.h"

class Demo : public ui::Control {
private:
  ui::Button button{sf::FloatRect(700, 400, 200, 100), "click to proceed"};

public:
  Demo() { }

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_DEMO_H
