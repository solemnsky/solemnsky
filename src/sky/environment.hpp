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
 * Set of static information that surrounds a Sky.
 */
#pragma once

namespace sky {

/**
 * Holder and asynchronous loader for pieces of static information extracted
 * from a .solx file, used to instantiate / add to the functionality /
 * display a Sky -- with, respectively, geometry data, scripts, and graphics
 * resources.
 */
class Environment {
 private:
  // State.

  // Loading coroutines.

 public:
  Environment(); // the null environment, useful for testing and sandboxes
  Environment(const std::string &filepath);

  // we want a way to selectively load the different elements of an Environment
};

}

