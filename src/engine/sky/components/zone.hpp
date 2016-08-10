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
/**
 * Some zone in the sky. A pickup, checkpoint, etc.
 */
#pragma once
#include "engine/multimedia.hpp"
#include "util/networked.hpp"
#include "engine/sky/physics/physics.hpp"
#include "component.hpp"
#include "engine/sky/physics/shape.hpp"

namespace sky {

/**
 * All the settings and state that defines an entity.
 */
struct ZoneState {
  ZoneState() = default;
  ZoneState(const FillStyle &fill,
            const Shape &shape,
            const Clamped cooldown,
            const float cooldownRate,
            const sf::Vector2f &pos);

  FillStyle fill;
  Shape shape;
  Cooldown cooldown;
  float cooldownRate;
  sf::Vector2f pos;

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(fill, cooldown, cooldownRate, pos, shape);
  }

};

/**
 * Delta for Entity.
 */
struct ZoneDelta {
  ZoneDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cooldown);
  }

  optional<Cooldown> cooldown;

};

/**
 * Some non-player entity.
 */
class Zone: public Component<ZoneState, ZoneDelta> {
  friend class Sky;
 private:
  ZoneState lastState;

 protected:
  // Component impl.
  void prePhysics() override final;
  void postPhysics(const TimeDiff delta) override final;

 public:
  Zone() = delete;
  Zone(const ZoneState &state, Physics &physics);

  // Networked impl.
  ZoneState captureInitializer() const override final;
  void applyDelta(const ZoneDelta &delta) override final;
  optional<ZoneDelta> collectDelta() override final;

};

}
