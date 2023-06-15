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
#include "filepath.hpp"

fs::path getTopPath(const std::string &path) {
  const char* envPath = std::getenv("SOLEMNSKY_RESOURCES");
  if (!envPath) {
    fs::path top = fs::system_complete("../../"); // back when I was testing from ./build/release/..
    top += fs::path(path);
    return top;
  }
  return fs::path(envPath) / fs::path(path);
}

fs::path getMediaPath(const std::string &path) {
  return getTopPath("media/" + path);
}

fs::path getEnvironmentPath(const std::string &path) {
  return getTopPath("environments/" + path);
}

fs::path getTestPath(const std::string &path) {
  return getTopPath("tests/" + path);
}

std::string getFilename(const fs::path &path) {
  return path.filename().filename().string(); // ahaha
}
