#include <ui/frame.h>
#include <sky/flight.h>
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
  using namespace detail;

  auto &state = parent->state;
  if (state) {
    // potentially switch orientation
    bool newOrientation = Angle(state->rot + 90) > 180;
    if (state->rotCtrl == 0) orientation = newOrientation;

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0),
             rndrParam.flipSpeed * delta);
    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    // rolling (when rotation control is active)
    approach<float>(rollState, state->rotCtrl,
                    rndrParam.rollSpeed * delta);

    roll = flipComponent + rndrParam.rollAmount * rollState;
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

void Render::renderBars(ui::Frame &f,
                        std::vector<std::pair<float, const sf::Color &>> bars,
                        sf::FloatRect area) {
  const float height = area.height / bars.size();

  sf::FloatRect barArea;
  barArea.height = height;
  barArea.left = area.left;
  for (int i = 0; i < bars.size(); i++) {
    const auto &bar = bars[i];
    barArea.top = area.top + i * height;
    barArea.width = area.width * bar.first;

    f.drawRect(barArea, bar.second);
  }
}

std::pair<float, const sf::Color &> mkBar(float x, const sf::Color &c) {
  return std::pair<float, const sf::Color &>(x, c); // like seriously?
};

void Render::renderPlane(
    ui::Frame &f, const int pid, sky::Plane &plane) {
  using namespace detail; // for rndrParam
  if (plane.state) {
    PlaneState &state = *plane.state;
    detail::PlaneAnimState &planeAnimState = animState.at(pid);

    const auto &hitbox = state.tuning.hitbox;

    f.withTransform(
        sf::Transform().translate(state.pos).rotate(state.rot), [&]() {
//      f.drawRect(-0.5f * hitbox, 0.5f * hitbox,
//                 state.stalled ? sf::Color::Red : sf::Color::Green);

      f.withTransform(
          sf::Transform().scale(state.afterburner, state.afterburner), [&]() {
        f.drawRect(rndrParam.afterburnArea,
                   sf::Color::Red);
      });

      sheet.drawIndexAtRoll(f, sf::Vector2f(rndrParam.spriteSize,
                                            rndrParam.spriteSize),
                            planeAnimState.roll);

    });

    f.withTransform(sf::Transform().translate(state.pos), [&]() {
      typedef std::pair<float, sf::Color &> Bar;

      const float airspeedStall = state.tuning.flight.threshold /
                                  state.tuning.flight.airspeedFactor;
      renderBars(
          f,
          {mkBar(state.throttle,
                 state.stalled ? rndrParam.throttleStall
                               : rndrParam.throttle),
           mkBar(state.stalled
                 ? Clamped(0, 1, (state.forwardVelocity()) /
                                 state.tuning.stall.threshold)
                 : (state.airspeed - airspeedStall) / (1 - airspeedStall),
                 rndrParam.health),
           mkBar(state.energy, rndrParam.energy)},
          rndrParam.barArea
      );

//      f.drawText(
//          {-100, -100},
//          {"airspeed:" + std::to_string(state.airspeed),
//           "forward vel: " +
//           std::to_string((int) std::round(state.forwardVelocity()))
//          }
//      );
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
  f.drawSprite(textureOf(Res::Title),
               {0, 0}, {0, 0, 1600, 900});
  f.drawSprite(textureOf(Res::Title),
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
