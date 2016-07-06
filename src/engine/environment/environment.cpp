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
#include "util/printer.hpp"
#include "environment.hpp"

namespace sky {

/**
 * Environment.
 */

void Environment::loadMap(const std::string &filepath) {
  assert(false); // Not implemented.
}

void Environment::loadGraphics(const std::string &filepath) {
  assert(false); // Not implemented.
}

void Environment::loadScripts(const std::string &filepath) {
  assert(false); // Not implemented.
}

void Environment::loadNullMap() {
  map.emplace();
}

void Environment::loadNullGraphics() {
  graphics.emplace();
}

void Environment::loadNullScripts() {
  scripts.emplace();
}

// TODO: complete this

Environment::Environment(const EnvironmentURL &url) :
    fileArchive(fs::path("not-a-path")),
    loadProgress(0),
    url(url) {
  if (url == "NULL") {
    appLog("Creating null environment.", LogOrigin::Engine);
    workerThread = std::thread([&]() { loadNullMap(); });
  } else {
    // if (filepath) {
    //   appLog("Creating environment " + inQuotes(url)
    //              + " with environment file " + filepath.get(),
    //          LogOrigin::Engine);
    //   workerThread = std::thread([&]() { loadMap(filepath.get()); });
    // } else {
    //   loadError = true;
    //   appLog("Creating environment in error state: could not find "
    //              "environment file for URL " + inQuotes(url));
    // }
  }

}

Environment::Environment() :
    Environment("NULL") {}

Environment::~Environment() {
  if (workerThread.joinable()) workerThread.join();
}

void Environment::loadMore(
    const bool needGraphics, const bool needScripts) {
  if (loadingIdle()) {
    assert(!loadError); // it's pointless to load when we have a load error
    workerThread = std::thread([&]() {
      if (url == "NULL") {
        if (needGraphics) loadNullGraphics();
        if (needScripts) loadNullScripts();
      } else {
//        if (needGraphics) loadGraphics(filepath.get());
//        if (needScripts) loadScripts(filepath.get());
      }
    });
  }
}

void Environment::waitForLoading() {
  workerThread.join();
}

bool Environment::loadingErrored() const {
  return loadError;
}

bool Environment::loadingIdle() const {
  return workerThread.joinable();
}

float Environment::loadingProgress() const {
  return loadProgress;
}

Map const *Environment::getMap() const {
  return map.get_ptr();
}

}
