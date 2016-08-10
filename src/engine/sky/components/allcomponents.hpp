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
 * Helper macros for DRYing up the management of all the different ComponentSet<>s contained in the Sky.
 */
#pragma once
#include "componentset.hpp"

/**
 * Inclusion of all component headers.
 */
#include "entity.hpp"
#include "explosion.hpp"
#include "homebase.hpp"
#include "zone.hpp"

// TODO: Make this DRY with CPP magic, especially if things start getting larger.

/**
 * Macros for operating over the various components.
 */

#define COMPONENT_INITS \
  ComponentSetInit<Entity> entities; \
  ComponentSetInit<Explosion> explosions; \
  ComponentSetInit<HomeBase> homeBases; \
  ComponentSetInit<Zone> zones;

#define COMPONENT_DELTAS \
  optional<ComponentSetDelta<Entity>> entities; \
  optional<ComponentSetDelta<Explosion>> explosions; \
  optional<ComponentSetDelta<HomeBase>> homeBases; \
  optional<ComponentSetDelta<Zone>> zones;

#define COMPONENT_SETS \
  ComponentSet<Entity> entities; \
  ComponentSet<Explosion> explosions; \
  ComponentSet<HomeBase> homeBases; \
  ComponentSet<Zone> zones;

