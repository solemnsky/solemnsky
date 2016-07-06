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
#include <cstdlib>
#include <unistd.h>
#include "util/printer.hpp"

/**
 * Directory.
 */

Directory::Directory(const std::string &name,
                     std::vector<fs::path> &&files,
                     std::vector<Directory> &&directories) :
    name(name),
    files(files),
    directories(directories) {}

optional<Directory> Directory::open(const fs::path &path) {
  if (!fs::exists(path)) return {};

  if (!fs::is_directory(path)) {
    return {};
  } else {
    std::vector<fs::path> files;
    std::vector<Directory> directories;

    fs::directory_iterator end;
    for (fs::directory_iterator itr(path);
         itr != end;
         ++itr) {
      const auto childPath = itr->path();
      if (const auto dir = Directory::open(childPath)) {
        directories.push_back(*dir);
      } else {
        files.push_back(childPath);
      }
    }

    return Directory(path.filename().string(),
                     std::move(files),
                     std::move(directories));
  }
}

/**
 * Archive.
 */

void Archive::doWork() {
  const auto filepath = this->archivePath.string();
  appLog("Unzipping archive: " + filepath, LogOrigin::App);

  if (!fs::exists(this->archivePath)) {
    appLog("Archive filepath does not exist!", LogOrigin::Error);
    this->done = true;
    return;
  }

  if (!fs::is_regular_file(this->archivePath)) {
    appLog("Archive filepath does not point to a regular file!", LogOrigin::Error);
    this->done = true;
    return;
  }

  appLog("Invoking 7zip...");

  fs::path workingDir(".unzip-tmp/" + this->archivePath.filename().string());
  fs::remove_all(workingDir);
  fs::create_directories(workingDir);

  // this is the alternative to getting libarchive working on all our
  // build platforms ... the choice was obvious
  chdir(workingDir.string().c_str());
  system(("7z x " + archivePath.string()).c_str());
  chdir("../..");

  if (const auto opened = Directory::open(workingDir))
    this->result.emplace(*opened);

  this->done = true;
}

Archive::Archive(const fs::path &archivePath) :
    done(false),
    archivePath(fs::system_complete(archivePath)) {}

void Archive::load() {
  workerThread = std::thread([&]() { this->doWork(); });
}

void Archive::finishLoading() {
  workerThread.join();
}

bool Archive::isDone() const {
  return done;
}

optional<Directory> Archive::getResult() const {
  return result;
}

