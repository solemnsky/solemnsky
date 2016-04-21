#include "sky/plane.h"
#include "client/elements/style.h"
#include "skyrender.h"
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
    rollState(0) { }

void PlaneGraphics::tick(const float delta) {
  if (plane.vital) {
    const auto &vital = plane.vital;

    // potentially switch orientation
    bool newOrientation = Angle(vital->state.physical.rot + 90) > 180;
    const Movement rotMovement = plane.controls.rotMovement();
    if (rotMovement == Movement::None) orientation = newOrientation;

    // flipping (when orientation changes)
    approach(flipState, (const float) (orientation ? 1 : 0),
             style.skyRender.flipSpeed * delta);

    // rolling (when rotation control is active)
    approach(rollState, movementValue(rotMovement),
             style.skyRender.rollSpeed * delta);
  }
}

Angle PlaneGraphics::roll() const {
  Angle flipComponent;
  if (orientation) flipComponent = 90 - flipState * 180;
  else flipComponent = 90 + flipState * 180;
  return flipComponent + style.skyRender.rollAmount * rollState;
}

void PlaneGraphics::kill() { }

void PlaneGraphics::spawn() {
  orientation = Angle(plane.vital->state.physical.rot + 90) > 180;
  flipState = 0;
  rollState = 0;
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

void SkyRender::renderProps(ui::Frame &f,
                            const Plane &plane) {
  for (auto &prop : plane.props) {
    f.withTransform(
        sf::Transform()
            .translate(prop.physical.pos)
            .rotate(prop.physical.rot), [&]() {
          f.drawRect(sf::Vector2f(-5, -5),
                     sf::Vector2f(5, 5),
                     sf::Color::White);
        });
  }
}

void SkyRender::renderPlaneGraphics(ui::Frame &f,
                                    const PlaneGraphics &graphics) {
  renderProps(f, graphics.plane);

  if (graphics.plane.isSpawned()) {
    const auto &state = graphics.plane.vital->state;
    const auto &tuning = graphics.plane.vital->tuning;

    f.withTransform(
        sf::Transform()
            .translate(state.physical.pos)
            .rotate(state.physical.rot), [&]() {
          f.withTransform(
              sf::Transform().scale(state.afterburner, state.afterburner),
              [&]() {
                f.drawRect(style.skyRender.afterburnArea, sf::Color::Red);
              });

          planeSheet.drawIndexAtRoll(
              f, sf::Vector2f(style.skyRender.spriteSize,
                              style.skyRender.spriteSize),
              graphics.roll());

          if (enableDebug) {
            const auto halfHitbox = 0.5f * tuning.hitbox;
            f.drawRect(-halfHitbox, halfHitbox, sf::Color(255, 255, 255, 100));
          }

        });

    f.withTransform(sf::Transform().translate(state.physical.pos), [&]() {
      const float airspeedStall = tuning.flight.threshold /
          tuning.flight.airspeedFactor;
      renderBars(
          f,
          {mkBar(state.throttle,
                 state.stalled ? style.skyRender.throttleStall
                               : style.skyRender.throttle),
           mkBar(state.stalled
                 ? clamp<float>(0, 1, ((state.forwardVelocity()) / tuning.stall
                         .threshold))
                 : (state.airspeed - airspeedStall) / (1 - airspeedStall),
                 style.skyRender.health),
           mkBar(state.energy, style.skyRender.energy)},
          style.skyRender.barArea
      );
    });
  } else {
    // TODO: death animation
  }
}

SkyRender::SkyRender(Arena &arena, Sky &sky, const bool enableDebug) :
    Subsystem(arena), sky(sky),
    enableDebug(enableDebug),
    planeSheet(ResID::PlayerSheet) {
  arena.forPlayers([&](Player &player) { registerPlayer(player); });
}

SkyRender::~SkyRender() { }

void SkyRender::registerPlayer(Player &player) {
  graphics.emplace(player.pid, sky.getPlane(player));
  player.data.push_back(&graphics.at(player.pid));
}

void SkyRender::unregisterPlayer(Player &player) {
  graphics.erase(player.pid);
}

void SkyRender::onTick(const float delta) {
  for (auto &pair : graphics) pair.second.tick(delta);
}

void SkyRender::render(ui::Frame &f, const sf::Vector2f &pos) {
  f.withTransform(
      sf::Transform().translate(
          {-findView(1600, sky.map.dimensions.x, pos.x),
           -findView(900, sky.map.dimensions.y, pos.y)}
      ),
      [&]() {
        // TODO: give the map visual data, including background
        f.drawSprite(textureOf(ResID::Title),
                     {0, 0}, {0, 0, 1600, 900});
        f.drawSprite(textureOf(ResID::Title),
                     {1600, 0}, {0, 0, 1600, 900});
        for (auto &pair: graphics) renderPlaneGraphics(f, pair.second);
      }
  );
}

}
