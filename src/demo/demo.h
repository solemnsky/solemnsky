/**
 * A cute demo for things.
 */
#ifndef SOLEMNSKY_DEMO_H
#define SOLEMNSKY_DEMO_H

#include "base/control.h"
#include "base/ui.h"

class Demo : public Control {
private:
  WrapControl<ui::Button>
      button{std::make_shared<ui::Button>(sf::Vector2f(50, 50)),
             WrapSettings(sf::Transform().translate(800, 450))};
  // hmm, best way of doing this?

public:
  void tick(float delta) override;
  void render(Frame &f) override;
  void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_DEMO_H
