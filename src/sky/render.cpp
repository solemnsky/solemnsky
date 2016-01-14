#include <ui/frame.h>
#include "render.h"

namespace sky {

/****
 * Render submethods.
 */

float RenderMan::findView(
    const float viewWidth,
    const float totalWidth,
    const float viewTarget) const {
  if (totalWidth < viewWidth) return (totalWidth / 2) - viewWidth;
  if (viewTarget - (viewWidth / 2) < 0) return 0;
  if (viewTarget + (viewWidth / 2) > totalWidth)
    return totalWidth - viewWidth;
  return viewTarget - (viewWidth / 2);
}

void RenderMan::renderPlane(
    ui::Frame &f, const int pid, sky::Plane &plane) {
  if (plane.state) {
    PlaneState &state = *plane.state;
    PlaneAnimState &animState = plane.animState;

    const auto &hitbox = state.tuning.hitbox;

    f.pushTransform(
        sf::Transform().translate(state.pos).rotate(state.rot).
            scale(-1, 1));

    f.drawRect(-0.5f * hitbox, 0.5f * hitbox, sf::Color::Red);

    sheet.drawIndexAtRoll(f, sf::Vector2f(200, 200), animState.roll);
    f.popTransform();

    f.withTransform(sf::Transform().translate(state.pos), [&]() {
      f.drawText({-100, -100}, {std::to_string(pid)});
    });
  } else {
//    f.drawText(sf::Vector2f(800, 200), {"You're dead, so sorry ..."}, 40,
//               sf::Color::Black);
  }
}

RenderMan::RenderMan(Sky *sky) :
    sky(sky),
    sheet(Res::PlayerSheet) { }


void RenderMan::render(ui::Frame &f, const sf::Vector2f &pos) {
  f.pushTransform(sf::Transform().translate(
      {-findView(1600, sky->physics.dims.x, pos.x),
       -findView(900, sky->physics.dims.y, pos.y)}
  ));

  // implicitly draw a background in the {3200, 900} region
  f.drawSprite(textureOf(Res::TitleScreen),
               {0, 0}, {0, 0, 1600, 900});
  f.drawSprite(textureOf(Res::TitleScreen),
               {1600, 0}, {0, 0, 1600, 900});

  for (auto &pair : sky->planes) {
    renderPlane(f, pair.first, *pair.second);
  }

  f.popTransform();
}

void RenderMan::tick(float delta) {
  for (auto &pair : sky->planes) {
    pair.second->tickAnim(delta);
  }
}

}
