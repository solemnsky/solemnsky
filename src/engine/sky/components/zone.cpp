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
#include "zone.hpp"
#include "entity.hpp"

namespace sky {

/**
 * ZoneState.
 */

sky::ZoneState::ZoneState(
    const FillStyle &fill,
    const Clamped cooldown,
    const float cooldownRate,
    const sf::Vector2f &pos) :
    fill(fill),
    cooldown(cooldown),
    cooldownRate(cooldownRate),
    pos(pos) { }

/**
 * Zone.
 */

void Zone::prePhysics() {

}

void Zone::postPhysics(const TimeDiff delta) {

}

Zone::Zone(const ZoneState &state, Physics &physics) :
    Component(state, physics) {
  // TODO: initialize some sort of box2d area for entrance checking?
}

ZoneState Zone::captureInitializer() const {
  return state;
}

void Zone::applyDelta(const ZoneDelta &delta) {
  if (delta.cooldown) {
    state.cooldown = delta.cooldown.get();
  }
}

optional<ZoneDelta> Zone::collectDelta() {
  ZoneDelta delta;
  if (lastState.cooldown != state.cooldown) {
    delta.cooldown = state.cooldown;
    lastState = state;
    return delta;
  }

  return {};
}

}
