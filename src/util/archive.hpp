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
 * Utilities to open and access zip archives.
 */
#pragma once
#include "util/types.hpp"
#include "util/threads.hpp"

/**
 * Handle to the directory of an opened archive, whose contents we can access.
 */
struct Directory {
 private:
  Directory(const std::string &directory);

  const std::string directory;

 public:
  Directory() = delete;

  // Name and contents of the directory.
  const std::string name;
  const std::vector<std::string> files; // Accessible filepaths.
  const std::vector<Directory> directories;

};

/**
 * Handle to the (asynchronous) process of opening an archive.
 */
class Archive {
 private:

  // Worker thread.
  void doWork();
  std::thread workerThread;

  // Progress state.
  float progress;
  bool done;

  // Result state.
  optional<std::string> error;
  optional<Directory> result;

 public:
  Archive(const std::string &filepath);

  const std::string filepath;

  // Loading proces.
  void load();
  void finishLoading();
  float getProgress() const;

  // When done, we present either a result or an error.
  bool isDone() const;
  optional<std::string> getError() const;
  optional<Directory> getResult() const;

};



