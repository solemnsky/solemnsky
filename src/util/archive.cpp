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
#include "archive.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

/**
 * Directory.
 */

Directory::Directory(std::string &&name,
                     std::vector<std::string> &&files,
                     std::vector<Directory> &&directories) :
  name(name),
  files(files),
  directories(directories) { }

optional<Directory> Directory::open(const std::string &filepath) {
  // fs::path fullpath(filepath);
  // if (!fs::is_directory(fullpath)) {
  //   return {};
  // } else {
  //   return Directory("stub", {}, {});
  // }
  return {};
}

/**
 * Archive.
 */

void Archive::doWork() {
  this->done = true;
  error.emplace("Not implemented");
}

Archive::Archive(const std::string &filepath) :
  progress(0),
  done(false),
  filepath(filepath) {}

void Archive::load() {
  workerThread = std::thread([&]() { this->doWork(); });
}

void Archive::finishLoading() {
  workerThread.join();
}

float Archive::getProgress() const {
  return progress;
}

bool Archive::isDone() const {
  return done;
}

optional<std::string> Archive::getError() const {
  return error;
}

optional<Directory> Archive::getResult() const {
  return result;
}

