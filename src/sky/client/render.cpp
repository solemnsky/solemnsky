#include <ui/frame.h>
#include "render.h"
#include "sky/sky.h"

namespace sky {
namespace detail {

/****
 * PlaneAnimState
 */

PlaneAnimState::PlaneAnimState() :
    roll(0),
    orientation(false),
    flipState(0),
    rollState(0) { }

void PlaneAnimState::tick(Plane *parent, const float delta) {
  auto &state = parent->state;
  if (state) {
    // potentially switch orientation
    bool newOrientation = Angle(state->rot + 90) > 180;
    if (state->rotCtrl == 0
        and newOrientation != orientation
        and state->stalled) {
      orientation = newOrientation;
    }

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0), 2 * delta);
    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    // rolling (when rotation control is active)
    approach<float>(rollState, state->rotCtrl,
                    detail::AnimSettings::rollSpeed * delta);

    roll = flipComponent + detail::AnimSettings::rollAmount * rollState;
  }
}

void PlaneAnimState::reset() {
  operator=((PlaneAnimState &&) PlaneAnimState());
}

}

/****
 * Render submethods.
 */

float Render::findView(
    const float viewWidth,
    const float totalWidth,
    const float viewTarget) const {
  if (totalWidth < viewWidth) return (totalWidth / 2) - viewWidth;
  if (viewTarget - (viewWidth / 2) < 0) return 0;
  if (viewTarget + (viewWidth / 2) > totalWidth)
    return totalWidth - viewWidth;
  return viewTarget - (viewWidth / 2);
}

void Render::renderPlane(
    ui::Frame &f, const int pid, sky::Plane &plane) {
  if (plane.state) {
    PlaneState &state = *plane.state;
    detail::PlaneAnimState &planeAnimState = animState.at(pid);

    const auto &hitbox = state.tuning.hitbox;

    f.pushTransform(
        sf::Transform().translate(state.pos).rotate(state.rot).
            scale(-1, 1));

    f.drawRect(-0.5f * hitbox, 0.5f * hitbox,
               state.stalled ? sf::Color::Red : sf::Color::Green);

    sheet.drawIndexAtRoll(f, sf::Vector2f(200, 200), planeAnimState.roll);
    f.popTransform();

    f.withTransform(sf::Transform().translate(state.pos), [&]() {
      f.drawText({-100, -100},
                 {std::to_string(pid),
                  state.stalled ? "stalled" : "flying"});
    });
  }
}

Render::Render(Sky *sky) :
    Subsystem(sky),
    sheet(Res::PlayerSheet) {
  CTOR_LOG("render subsystem");
}

Render::~Render() {
  DTOR_LOG("render subsystem");
}

void Render::render(ui::Frame &f, const sf::Vector2f &pos) {
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

/****
 * Subsystem listeners.
 */

void Render::tick(float delta) {
  for (auto &pair : animState)
    pair.second.tick(sky->getPlane(pair.first), delta);
}

void Render::joinPlane(const PID pid, Plane *plane) {
  animState[pid] = detail::PlaneAnimState();
}

void Render::quitPlane(const PID pid) {
  animState.erase(pid);
}

void Render::spawnPlane(const PID pid, Plane *plane) {
  animState.at(pid).reset();
}

void Render::killPlane(const PID pid, Plane *plane) { }

}
