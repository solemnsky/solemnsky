/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "engine/sky/participation.hpp"
#include "client/elements/style.hpp"
#include "skyrender.hpp"

namespace sky {

/**
 * PlayerGraphics.
 */

PlaneGraphics::PlaneGraphics(const Player &player, const Participation &plane) :
    participation(plane),
    player(player),
    orientation(false),
    flipState(0),
    rollState(0) { }

void PlaneGraphics::tick(const float delta) {
  if (auto &plane = participation.plane) {
    // potentially switch orientation
    bool newOrientation = Angle(plane->getState().physical.rot + 90) > 180;
    const Movement rotMovement = participation.getControls().rotMovement();
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

void PlaneGraphics::onSpawn() {
  orientation =
      Angle(participation.plane->getState().physical.rot + 90) > 180;
  flipState = 0;
  rollState = 0;
}

void PlaneGraphics::onKill() {

}

/**
 * RenderSystem.
 */

float SkyRender::findView(
    const float viewWidth,
    const float totalWidth,
    const float viewTarget) const {
  if (totalWidth < viewWidth) return ((totalWidth - viewWidth) / 2);
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
  for (size_t i = 0; i < bars.size(); i++) {
    const auto &bar = bars[i];
    barArea.top = area.top + i * height;
    barArea.width = area.width * bar.first;

    f.drawRect(barArea, bar.second);
  }
}

std::pair<float, const sf::Color &> mkBar(float x, const sf::Color &c) {
  return std::pair<float, const sf::Color &>(x, c);
}

void SkyRender::renderPlaneGraphics(ui::Frame &f,
                                    const PlaneGraphics &graphics) {
  if (auto &plane = graphics.participation.plane) {
    auto &state = plane->getState();
    auto &tuning = plane->getTuning();

    const float scaleFactor = style.skyRender.planeGraphicsScale
        * tuning.hitbox.x / 200;

    f.withTransform(
        sf::Transform()
            .translate(state.physical.pos)
            .rotate(state.physical.rot), [&]() {

      f.withTransform(sf::Transform().scale(scaleFactor, scaleFactor), [&]() {
        // Plane graphics, scaled down so the plane's length is 200 px from this perspective.
        f.withAlpha(state.afterburner, [&]() {
          f.drawRect(style.skyRender.afterburnArea,
                     sf::Color::Red);
        });
        planeSheet.drawIndexAtRoll(
            f, sf::Vector2f(200, 200), graphics.roll());
      });

      if (enableDebug) {
        // Debug graphics.
        const auto halfHitbox = 0.5f * tuning.hitbox;
        f.drawRect(-halfHitbox, halfHitbox, sf::Color(255, 255, 255, 100));
      }
    });

    f.withTransform(sf::Transform().translate(state.physical.pos), [&]() {
      const float airspeedStall = tuning.flight.threshold /
          tuning.flight.airspeedFactor;
      f.drawText({0, -style.skyRender.barArea.top - style.base.normalFontSize},
                 graphics.player.getNickname(),
                 (graphics.player.getTeam() == sky::Team::Red) ? sf::Color::Red
                                                               : sf::Color::Blue,
                 style.base.centeredText, resources.defaultFont);
      renderBars(
          f,
          {mkBar(state.throttle,
                 state.stalled ? style.skyRender.throttleStall
                               : style.skyRender.throttle),
           mkBar(state.stalled
                 ? clamp<float>(0, 1, ((state.forwardVelocity()) /
                         tuning.stall.threshold))
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

void SkyRender::renderMap(ui::Frame &f) {
  const sky::Map &map = sky.getMap();
  const auto &dims = map.getDimensions();

  f.drawRect({0, 0}, dims, style.menu.pageBgColor);
  f.drawSprite(resources.getTexture(ui::TextureID::Title),
               {(dims.x / 2) - 800, 0},
               {0, 0, 1600, 900});

  for (const auto &obstacle : map.getObstacles()) {
    f.withTransform(sf::Transform().translate(obstacle.pos), [&]() {
      f.drawPolyOutline(obstacle.localVertices, sf::Color::White);
    });
  }
}

SkyRender::SkyRender(ClientShared &shared,
                     const ui::AppResources &resources,
                     Arena &arena,
                     const Sky &sky) :
    ClientComponent(shared),
    Subsystem(arena),
    sky(sky),
    resources(resources),
    sheet(ui::TextureID::PlayerSheet),
    planeSheet(resources.getTextureData(sheet).spritesheetForm.get(),
               resources.getTexture(sheet)),
    enableDebug(shared.references.settings.enableDebug) {
  arena.forPlayers([&](Player &player) { registerPlayer(player); });
}

void SkyRender::registerPlayer(Player &player) {
  setPlayerData(
      player,
      graphics.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(player.pid),
          std::forward_as_tuple(
              player, sky.getParticipation(player))).first->second);
}

void SkyRender::unregisterPlayer(Player &player) {
  graphics.erase(player.pid);
}

void SkyRender::onTick(const float delta) {
  for (auto &pair : graphics) pair.second.tick(delta);
}


void SkyRender::onSpawn(Player &player) {
  getPlayerData(player).onSpawn();
}

void SkyRender::onKill(Player &player) {
  getPlayerData(player).onKill();
}

void SkyRender::onChangeSettings(const ui::SettingsDelta &settings) {
  if (settings.enableDebug) enableDebug = settings.enableDebug.get();
}

void SkyRender::render(ui::Frame &f, const sf::Vector2f &pos) {
  const auto &map = sky.getMap();
  const auto &dims = map.getDimensions();
  const auto &viewScale = sky.settings.getViewscale();

  f.withTransform(
      sf::Transform()
          .scale(viewScale, viewScale)
          .translate(
              {-findView(1600 / viewScale, dims.x, pos.x),
               -findView(900 / viewScale, dims.y, pos.y)}),
      [&]() {
        renderMap(f);
        for (auto &pair: graphics) renderPlaneGraphics(f, pair.second);
      }
  );
}

}
