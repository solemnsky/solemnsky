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
 * Ubiquotous types for use all over the engine.
 */
#pragma once
#include "util/types.hpp"

namespace sky {

/**
 * Uniform resource locator for Environments.
 * E.g.: vanilla/ball_asteroids, or some_fun_mod/interesting_map
 * Mapped to the filesystem location of a .sky file.
 */
using EnvironmentURL = std::string;

/**
 * Degrees of action to with which to control Participation. Passed along with a state,
 * corresponding to whether the action was begun or ended.
 */
enum class Action {
  Thrust,
  Reverse,
  Left,
  Right,
  Primary,
  Secondary,
  Special,
  Suicide,
  MAX
};

/**
 * Reference to a graphics entity, either from the default resources or from
 * an environment  visual component.
 */
struct VisualEntity {
  VisualEntity(const bool isDefault, const PID index);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(isDefault, index);
  }

  bool isDefault; // If it's in the default resources table -- otherwise, it's in the Visuals component.
  PID index;

};

}
