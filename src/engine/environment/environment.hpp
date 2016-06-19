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
#include "util/types.hpp"
#include "envgraphics.hpp"
#include "envscripts.hpp"
#include "map.hpp"
#include "util/threads.hpp"

namespace sky {

/**
 * Uniform resource locator for Environments.
 * E.g.: vanilla/ball_asteroids, or some_fun_mod/interesting_map
 * Mapped to the filesystem location of a .solx file.
 */
using EnvironmentURL = std::string;

/**
 * Holder and asynchronous loader for pieces of static information extracted
 * from a .sky file, used to instantiate / add to the functionality /
 * display a Sky -- with, respectively, geometry data, scripts, and graphics
 * resources.
 */
class Environment {
 private:
  // Filepath.
  const optional<std::string> filepath;

  // State.
  bool loadError;
  float loadProgress;
  optional<EnvGraphics> graphics;
  optional<EnvScripts> scripts;
  optional<Map> map;

  std::thread workerThread;

  // Loading submethods.
  void loadMap(const std::string &filepath);
  void loadGraphics(const std::string &filepath);
  void loadScripts(const std::string &filepath);

  // Null loading submethods.
  void loadNullMap();
  void loadNullGraphics();
  void loadNullScripts();

 public:
  Environment(const EnvironmentURL &url);
  // The null environment, useful for testing and sandboxes.
  // The default ctor is equilivant to supplying a URL of "NULL".
  Environment();
  ~Environment();

  const EnvironmentURL url;

  // Loading.
  void loadMore(const bool needGraphics, const bool needScripts);
  void waitForLoading();

  // Load status.
  bool loadingErrored() const;
  bool loadingIdle() const;
  float loadingProgress() const;

  // Accessing loaded resources.
  Map const *getMap() const;

};

}

