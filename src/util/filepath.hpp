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
 * Canonical filepath utilities. Includes the boost filesystem library.
 */
#pragma once
#include <boost/filesystem.hpp>

// alias to boost filesystem library
namespace fs = boost::filesystem;

// MEMO: the output of these functions can depend on the release platform.

/**
 * Access a path relative to the media directory.
 */
fs::path getMediaPath(const std::string &path);

/**
 * Access a path relative to the environment directory.
 */
fs::path getEnvironmentPath(const std::string &path);

/**
 * Access a path relative to the test file directory.
 */
fs::path getTestPath(const std::string &path);

/**
 * Access the filename of a path as a string.
 */
std::string getFilename(const fs::path &path);
