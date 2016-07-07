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
#include <util/methods.hpp>
#include "util/printer.hpp"
#include "util/methods.hpp"
#include "environment.hpp"

namespace sky {

/**
 * Environment.
 */

std::string Environment::describeComponent(const Component c) {
  switch (c) {
    case Component::Map: return "map";
    case Component::Graphics: return "map";
    case Component::Scripts: return "scripts";
  }
  throw enum_error();
}

std::string Environment::describeComponentLoading(const Component c) {
  return "Loading environment " + describeComponent(c) + " component...";
}

std::string Environment::describeComponentLoadingNull(const Component c) {
  return "Creating null " + describeComponent(c)
      + " component for environment...";
}

std::string Environment::describeComponentMissing(const Component c) {
  return "Environment " + describeComponent(c) + " component data not found!";
}

std::string Environment::describeComponentMalformed(const Component c) {
  return "Environment " + describeComponent(c) + " component data appears to be malformed!";
}

void Environment::loadMap(const fs::path &mapPath) {
  appLog(describeComponentLoading(Component::Map), LogOrigin::Engine);
  std::ifstream mapFile(mapPath.string());
  if (auto map = Map::load(mapFile)) {
    this->map.emplace(std::move(map.get()));
  } else {
    appLog(describeComponentMalformed(Component::Map), LogOrigin::Error);
    loadError = true;
  }
}

void Environment::loadGraphics(const fs::path &graphicsPath) {
  appLog(describeComponentLoading(Component::Graphics), LogOrigin::Engine);
  assert(false);
}

void Environment::loadScripts(const fs::path &scriptPath) {
  appLog(describeComponentLoading(Component::Scripts), LogOrigin::Engine);
  assert(false);
}

void Environment::loadNullMap() {
  appLog(describeComponentLoadingNull(Component::Map), LogOrigin::Engine);
  map.emplace();
}

void Environment::loadNullGraphics() {
  appLog(describeComponentLoadingNull(Component::Graphics), LogOrigin::Engine);
  graphics.emplace();
}

void Environment::loadNullScripts() {
  appLog(describeComponentLoadingNull(Component::Scripts), LogOrigin::Engine);
  scripts.emplace();
}

Environment::Environment(const EnvironmentURL &url) :
    archivePath(getEnvironmentPath(url + ".sky")),
    fileArchive(archivePath),
    loadProgress(0),
    loadError(false),
    url(url) {
  if (url == "NULL") {
    appLog("Creating null environment.", LogOrigin::Engine);
    workerThread = std::thread([&]() { loadNullMap(); });
  } else {
    appLog("Creating environment " + inQuotes(url)
               + " with environment file " + archivePath.string(),
           LogOrigin::Engine);

    workerThread = std::thread([&]() {
      fileArchive.load();
      if (const auto dir = fileArchive.getResult()) {
        if (const auto mapPath = dir->getTopFile("map.json")) {
          loadMap(mapPath.get());
        } else {
          appLog(describeComponentMissing(Component::Map), LogOrigin::Error);
          loadError = true;
        }
      } else {
        appLog("Could not open environment archive at path "
                   + archivePath.string());
        loadError = true;
      }
    });
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
    assert(!loadError);
    // it's pointless to load when we have a load error
    // the user should handle the error case

    workerThread = std::thread([&]() {
      if (url == "NULL") {
        if (needGraphics) loadNullGraphics();
        if (needScripts) loadNullScripts();
      } else {
        const auto dir = *this->fileArchive.getResult();
        if (needGraphics) {
          if (const auto graphicsFile = dir.getTopFile("graphics.json")) {
            loadGraphics(graphicsFile.get());
          } else {
            appLog(describeComponentMissing(Component::Graphics),
                   LogOrigin::Error);
            loadError = true;
          }
        }
        if (needScripts) {
          if (const auto scriptFile = dir.getTopFile("scripts.clj")) {
            loadScripts(scriptFile.get());
          } else {
            appLog(describeComponentMissing(Component::Scripts),
                   LogOrigin::Error);
            loadError = true;
          }
        }
      }
    });
  }
}

void Environment::waitForLoading() {
  if (workerThread.joinable())
    workerThread.join();
}

bool Environment::loadingErrored() const {
  return loadError;
}

bool Environment::loadingIdle() const {
  return !workerThread.joinable();
}

float Environment::loadingProgress() const {
  return loadProgress;
}

Map const *Environment::getMap() const {
  return map.get_ptr();
}

EnvGraphics const *Environment::getGraphics() const {
  return graphics.get_ptr();
}

EnvScripts const *Environment::getScripts() const {
  return scripts.get_ptr();
}

}
