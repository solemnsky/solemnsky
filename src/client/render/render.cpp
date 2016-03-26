#include "sky/plane.h"
#include "client/elements/style.h"
#include "render.h"
#include "sky/sky.h"
#include "util/methods.h"
#include "util/methods.h"

namespace sky {

/**
 * PlayerGraphics.
 */

PlaneGraphics::PlaneGraphics(const Plane &plane) :
    plane(plane),
    orientation(false),
    flipState(0),
    rollState(0),
    life(int(bool(plane.vital))),
    wasDead(true) {
  if (plane.vital) onRespawn();
  wasDead = false;
  onRespawn();
}

void PlaneGraphics::onRespawn() {
  orientation = Angle(plane.vital->state.rot + 90) > 180;
  flipState = 0;
  rollState = 0;
  life = 1;
  wasDead = false;
}

void PlaneGraphics::tick(const float delta) {
  if (plane.vital) {
    if (wasDead) onRespawn();
    const auto &vital = plane.vital;

    // potentially switch orientation
    bool newOrientation = Angle(vital->state.rot + 90) > 180;
    if (vital->state.rotCtrl == Movement::None) orientation = newOrientation;

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0),
             style.skyRender.flipSpeed * delta);
    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    // rolling (when rotation control is active)
    approach(rollState, movementValue(vital->state.rotCtrl),
             style.skyRender.rollSpeed * delta);

    roll = flipComponent + style.skyRender.rollAmount * rollState;

  } else {
    life -= style.skyRender.deathRate * delta;
    wasDead = true;
  }
}

/**
 * RenderSystem.
 */

float SkyRender::findView(
    const float viewWidth,
    const float totalWidth,
    const float viewTarget) const {
  if (totalWidth < viewWidth) return (totalWidth / 2) - viewWidth;
  if (viewTarget - (viewWidth / 2) < 0) return 0;
  if (viewTarget + (viewWidth / 2) > totalWidth)
    return totalWidth - viewWidth;
  return viewTarget - (viewWidth / 2);
}

void SkyRender::renderBars(ui::Frame &f,
                           std::vector<std::pair<float,
                                                 const sf::Color &>> bars,
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
  return std::pair<float, const sf::Color &>(x, c);
};

void SkyRender::renderPlaneGraphics(ui::Frame &f,
                                    const PlaneGraphics &graphics) {
  if (graphics.parent) {
    const auto &state = graphics.parent->state;
    const auto &tuning = graphics.parent->tuning;

    f.withTransform(
        sf::Transform().translate(state.pos).rotate(state.rot), [&]() {

          f.withTransform(
              sf::Transform().scale(state.afterburner, state.afterburner),
              [&]() {
                f.drawRect(rndrParam.afterburnArea, sf::Color::Red);
              });

          sheet.drawIndexAtRoll(
              f, sf::Vector2f(rndrParam.spriteSize, rndrParam.spriteSize),
              graphics.roll);
        });

    f.withTransform(sf::Transform().translate(state.pos), [&]() {
      const float airspeedStall = tuning.flight.threshold /
          tuning.flight.airspeedFactor;
      renderBars(
          f,
          {mkBar(state.throttle,
                 state.stalled ? rndrParam.throttleStall
                               : rndrParam.throttle),
           mkBar(state.stalled
                 ? clamp<float>(0, 1, ((state.forwardVelocity()) / tuning.stall
                         .threshold))
                 : (state.airspeed - airspeedStall) / (1 - airspeedStall),
                 rndrParam.health),
           mkBar(state.energy, rndrParam.energy)},
          rndrParam.barArea
      );
    });
  } else {
    // plane is dead
  }
}

SkyRender::SkyRender(const Arena *arena, const Sky *sky) :
    Subsystem(arena), sky(sky),
    sheet(ResID::PlayerSheet) {
}

SkyRender::~SkyRender() {
}


void SkyRender::onTick(const float delta) {
  std::remove_if(graphics.begin(), graphics.end(), [delta]
      (PlaneGraphics &planeGraphics) {
    planeGraphics.tick(delta);
    return planeGraphics.destroyed;
  });
}

void SkyRender::addPlane(const PID pid, PlaneVital &plane) {
  graphics.emplace_back(pid, plane);
}

void SkyRender::removePlane(const PID pid) {
  for (auto &planeGraphics : graphics) planeGraphics.removePlane(pid);
}

void SkyRender::render(ui::Frame &f, const sf::Vector2f &pos) {
  f.pushTransform(sf::Transform().translate(
      {-findView(1600, sky->map.dimensions.x, pos.x),
       -findView(900, sky->map.dimensions.y, pos.y)}
  ));

  // TODO: give sky->map visual data, including background
  f.drawSprite(textureOf(ResID::Title),
               {0, 0}, {0, 0, 1600, 900});
  f.drawSprite(textureOf(ResID::Title),
               {1600, 0}, {0, 0, 1600, 900});

  for (auto &planeGraphics : graphics)
    renderPlaneGraphics(f, planeGraphics);

  f.popTransform();
}

}
