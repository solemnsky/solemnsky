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
 * Some non-player entity in a Sky.
 */
#pragma once
#include "util/types.hpp"
#include "engine/multimedia.hpp"

namespace sky {

/**
 * Data that defines the entity.
 */
class EntityData {
  sf::Vector2f pos;
  std::vector<sf::Vector2f> localVerticies;
  bool collide, fixed;

  FillType fillType;

};

/**
 * Delta type for Entity.
 */
class EntityDelta {

};

/**
 * A non-player entity, defined by an EntityData.
 */
class Entity : public Networked<EntityData, EntityDelta> {

};

}
