#include <ui/frame.h>
#include "render.h"

namespace sky {

RenderMan::RenderMan(Sky *sky) :
    sky(sky),
    sheet(Res::PlayerSheet) { }

void RenderMan::render(ui::Frame &f, sf::Vector2f pos) {
  if (Plane *plane = sky->getPlane(0)) {
    if (auto &state = plane->state) {
      const auto &hitbox = state->tuning.hitbox;
      f.pushTransform(
          sf::Transform().translate(state->pos).rotate(state->rot));
      f.drawRect(-0.5f * hitbox, 0.5f * hitbox, sf::Color::Red);
      sheet.drawIndex(f, sf::Vector2f(200, 200), 7);
      f.popTransform();
    } else {
      f.drawText(sf::Vector2f(800, 200), {"You're dead, so sorry ..."}, 40,
                 sf::Color::Black);
    }
  }
}

void RenderMan::tick(float delta) {

}
}
