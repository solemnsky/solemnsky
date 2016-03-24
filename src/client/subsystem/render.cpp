#include "sky/plane.h"
#include "render.h"
#include "sky/sky.h"
#include "util/methods.h"
#include "util/methods.h"

namespace sky {

/**
 * PlaneGraphics.
 */

PlaneGraphics::PlaneGraphics(const PID pid, const PlaneVital &parent) :
    pid(pid),
    parent(&parent),
    roll(90),
    orientation(Angle(parent.state.rot + 90) > 180),
    flipState(0),
    rollState(0),
    destroyed(false) { }

PlaneGraphics &PlaneGraphics::operator=(PlaneGraphics &&graphics) {
  pid = graphics.pid;
  parent = graphics.parent;
  roll = graphics.roll;
  orientation = graphics.orientation;
  flipState = graphics.flipState;
  rollState = graphics.rollState;
  destroyed = graphics.destroyed;
  return *this;
}

void PlaneGraphics::tick(const float delta) {
  if (parent) {
    // potentially switch orientation
    bool newOrientation = Angle(parent->state.rot + 90) > 180;
    if (parent->state.rotCtrl == Movement::None) orientation = newOrientation;

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0),
             rndrParam.flipSpeed * delta);
    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    // rolling (when rotation control is active)
    approach(rollState, movementValue(parent->state.rotCtrl),
             rndrParam.rollSpeed * delta);

    roll = flipComponent + rndrParam.rollAmount * rollState;
  } else {
    // TODO: death animation: store last plane state, etc
    destroyed = true;
  }
}

void PlaneGraphics::removePlane(const PID removedPid) {
  if (pid) {
    if (removedPid == *pid) {
      parent = nullptr;
      destroyed = true;
    }
  }
}

/**
 * RenderSystem.
 */

float RenderSystem::findView(
    const float viewWidth,
    const float totalWidth,
    const float viewTarget) const {
  if (totalWidth < viewWidth) return (totalWidth / 2) - viewWidth;
  if (viewTarget - (viewWidth / 2) < 0) return 0;
  if (viewTarget + (viewWidth / 2) > totalWidth)
    return totalWidth - viewWidth;
  return viewTarget - (viewWidth / 2);
}

void RenderSystem::renderBars(ui::Frame &f,
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

void RenderSystem::renderPlaneGraphics(ui::Frame &f,
                                       const PlaneGraphics &graphics) {
  if (graphics.parent) {
    const auto &state = graphics.parent->state;
    const auto &tuning = graphics.parent->tuning;

    f.withTransform(
        sf::Transform().translate(state.pos).rotate(state.rot), [&]() {

      f.withTransform(
          sf::Transform().scale(state.afterburner, state.afterburner), [&]() {
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

RenderSystem::RenderSystem(const Arena *arena, const Sky *sky) :
    Subsystem(arena), sky(sky),
    sheet(ResID::PlayerSheet) {
}

RenderSystem::~RenderSystem() {
}


void RenderSystem::tick(float delta) {
  std::remove_if(graphics.begin(), graphics.end(), [delta]
      (PlaneGraphics &planeGraphics) {
    planeGraphics.tick(delta);
    return planeGraphics.destroyed;
  });
}

void RenderSystem::addPlane(const PID pid, PlaneVital &plane) {
  graphics.emplace_back(pid, plane);
}

void RenderSystem::removePlane(const PID pid) {
  for (auto &planeGraphics : graphics) planeGraphics.removePlane(pid);
}

void RenderSystem::render(ui::Frame &f, const sf::Vector2f &pos) {
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
