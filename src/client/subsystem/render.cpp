#include "sky/flight.h"
#include "render.h"
#include "sky/sky.h"
#include "util/methods.h"
#include "util/methods.h"

namespace sky {
namespace detail {

/****
 * PlaneAnimState
 */

PlaneAnimState::PlaneAnimState() :
    roll(0),
    orientation(false),
    flipState(0),
    rollState(90) { }

void PlaneAnimState::spawn(const PlaneVital &vital) {
  roll = 90;
  orientation = Angle(vital.rot + 90) > 180;
  flipState = 0;
  rollState = 0;
}

void PlaneAnimState::tick(sky::PlaneHandle *parent, const float delta) {
  using namespace detail;

  auto vstate = parent->state.vital;

  if (vstate) {
    // potentially switch orientation
    bool newOrientation = Angle(vstate->rot + 90) > 180;
    if (vstate->rotCtrl == 0) orientation = newOrientation;

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0),
             rndrParam.flipSpeed * delta);
    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    // rolling (when rotation control is active)
    approach<float>(rollState, vstate->rotCtrl,
                    rndrParam.rollSpeed * delta);

    roll = flipComponent + rndrParam.rollAmount * rollState;
  }
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
    ui::Frame &f, const PID pid, const PlaneHandle &plane) {
  using namespace detail; // for rndrParam
  if (plane.state.vital) {
    const PlaneVital &vstate = *plane.state.vital;
    const PlaneTuning &tuning = plane.state.tuning;

    detail::PlaneAnimState &planeAnimState = animState.at(pid);

    f.withTransform(
        sf::Transform().translate(vstate.pos).rotate(vstate.rot), [&]() {

      f.withTransform(
          sf::Transform().scale(vstate.afterburner, vstate.afterburner), [&]() {
        f.drawRect(rndrParam.afterburnArea,
                   sf::Color::Red);
      });

      sheet.drawIndexAtRoll(f, sf::Vector2f(rndrParam.spriteSize,
                                            rndrParam.spriteSize),
                            planeAnimState.roll);

    });

    f.withTransform(sf::Transform().translate(vstate.pos), [&]() {
      typedef std::pair<float, sf::Color &> Bar;

      const float airspeedStall = tuning.flight.threshold /
                                  tuning.flight.airspeedFactor;
      renderBars(
          f,
          {mkBar(vstate.throttle,
                 vstate.stalled ? rndrParam.throttleStall
                                : rndrParam.throttle),
           mkBar(vstate.stalled
                 ? Clamped(0, 1, (vstate.forwardVelocity()) /
                                 tuning.stall.threshold)
                 : (vstate.airspeed - airspeedStall) / (1 - airspeedStall),
                 rndrParam.health),
           mkBar(vstate.energy, rndrParam.energy)},
          rndrParam.barArea
      );
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
      {-findView(1600, sky->map.dimensions.x, pos.x),
       -findView(900, sky->map.dimensions.y, pos.y)}
  ));

  // implicitly draw a background in the {3200, 900} region
  f.drawSprite(textureOf(Res::Title),
               {0, 0}, {0, 0, 1600, 900});
  f.drawSprite(textureOf(Res::Title),
               {1600, 0}, {0, 0, 1600, 900});

  for (auto &pair : sky->planes) {
    renderPlane(f, pair.first, pair.second);
  }

  f.popTransform();
}

/****
 * Subsystem listeners.
 */

void Render::tick(float delta) {
  for (auto &pair : animState)
    pair.second.tick(sky->getPlaneHandle(pair.first), delta);
}

void Render::joinPlane(const PID pid, PlaneHandle &plane) {
  animState.emplace(pid, detail::PlaneAnimState());
}

void Render::quitPlane(const PID pid) {
  animState.erase(pid);
}

void Render::spawnPlane(const PID pid, PlaneHandle &plane) {
  animState.at(pid).spawn(*plane.state.vital);
}

void Render::killPlane(const PID pid, PlaneHandle &plane) { }

}
