#include <ui/frame.h>
#include "render.h"

namespace sky {

RenderMan::RenderMan(Sky *sky) :
    sky(sky),
    sheet(Res::PlayerSheet) { }

void RenderMan::render(ui::Frame &f, sf::Vector2f pos) {
  for (auto &pair : sky->planes) {
    const int pid = pair.first;
    const std::unique_ptr<Plane> &plane = pair.second;

    if (plane->state) {
      PlaneState &state = *plane->state;
      PlaneAnimState &animState = plane->animState;

      const auto &hitbox = state.tuning.hitbox;

      f.pushTransform(
          sf::Transform().translate(state.pos).rotate(state.rot).
              scale(-1, 1));

      f.drawRect(-0.5f * hitbox, 0.5f * hitbox, sf::Color::Red);

      sheet.drawIndexAtRoll(f, sf::Vector2f(200, 200), animState.roll);

      f.popTransform();

      f.drawText(state.pos + (sf::Vector2f) {-100, -100},
                 {std::to_string(pid)});
    } else {
      f.drawText(sf::Vector2f(800, 200), {"You're dead, so sorry ..."}, 40,
                 sf::Color::Black);
    }
  }
}

void RenderMan::tick(float delta) {
  for (auto &pair : sky->planes) {
    pair.second->tickAnim(delta);
  }
}
}
