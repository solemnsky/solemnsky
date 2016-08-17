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
#include <fstream>
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
    case Component::Visuals: return "visuals";
    case Component::Mechanics: return "mechanics";
  }
  throw enum_error();
}

std::string Environment::describeComponentLoading(const Component c) {
  return "Loading environment " + describeComponent(c) + " component...";
}

std::string Environment::describeComponentDone(const Environment::Component c) {
  return "Loaded " + describeComponent(c) + " component!";
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

void Environment::loadMap(const fs::path &path) {
  appLog(describeComponentLoading(Component::Map), LogOrigin::Engine);
  std::ifstream mapFile(path.string());
  if (auto map = Map::load(mapFile)) {
    this->map.emplace(std::move(map.get()));
  } else {
    appLog(describeComponentMalformed(Component::Map), LogOrigin::Error);
    loadError = true;
  }
  appLog(describeComponentDone(Component::Map), LogOrigin::Engine);
}

void Environment::loadMechanics(const fs::path &path) {
  appLog(describeComponentLoading(Component::Mechanics), LogOrigin::Engine);
  mechanics.emplace();
  appLog(describeComponentDone(Component::Mechanics), LogOrigin::Engine);
}

void Environment::loadVisuals(const fs::path &path) {
  appLog(describeComponentLoading(Component::Visuals), LogOrigin::Engine);
  visuals.emplace();
  appLog(describeComponentDone(Component::Visuals), LogOrigin::Engine);
}

void Environment::loadNullMap() {
  appLog(describeComponentLoadingNull(Component::Map), LogOrigin::Engine);
  map.emplace();
  sf::sleep(sf::milliseconds(20));
  appLog(describeComponentDone(Component::Map), LogOrigin::Engine);
}

void Environment::loadNullMechanics() {
  appLog(describeComponentLoadingNull(Component::Mechanics), LogOrigin::Engine);
  mechanics.emplace();
  sf::sleep(sf::milliseconds(20));
  appLog(describeComponentDone(Component::Mechanics), LogOrigin::Engine);
}

void Environment::loadNullVisuals() {
  appLog(describeComponentLoadingNull(Component::Visuals), LogOrigin::Engine);
  visuals.emplace();
  sf::sleep(sf::milliseconds(20));
  appLog(describeComponentDone(Component::Visuals), LogOrigin::Engine);
}

Environment::Environment(const EnvironmentURL &url) :
    archivePath(fs::system_complete(getEnvironmentPath(url + ".sky"))),
    fileArchive(archivePath),
    workerRunning(false),
    loadError(false),
    loadProgress(0),
    url(url) {
  if (url == "NULL") {
    appLog("Creating null environment.", LogOrigin::Engine);
    workerThread = std::thread([&]() { loadNullMap(); });
  } else {
    appLog("Creating environment " + inQuotes(url)
               + " with environment file " + archivePath.string(),
           LogOrigin::Engine);

    workerRunning = true;
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
                   + archivePath.string(), LogOrigin::Error);
        loadError = true;
      }

      workerRunning = false;
    });
  }
}

Environment::Environment() :
    Environment("NULL") {}

Environment::~Environment() {
  joinWorker();
}

void Environment::loadMore(
    const bool needVisuals, const bool needMechanics) {
  if (loadingIdle()) {
    joinWorker();
    assert(!loadError);
    assert(imply(needVisuals, !visuals));
    assert(imply(needMechanics, !mechanics));

    appLog("Beginning secondary environment loading.", LogOrigin::Engine);

    workerRunning = true;
    workerThread = std::thread([&, needVisuals, needMechanics]() {
      if (url == "NULL") {
        if (needVisuals) loadNullVisuals();
        if (needMechanics) loadNullMechanics();
      } else {
        const auto dir = *this->fileArchive.getResult();

        if (needVisuals) {
          if (const auto visualFile = dir.getTopFile("graphics.json")) {
            loadVisuals(visualFile.get());
          } else {
            appLog(describeComponentMissing(Component::Visuals),
                   LogOrigin::Error);
            loadError = true;
          }
        }

        if (needMechanics) {
          if (const auto mechanicsFile = dir.getTopFile("mechanics.json")) {
            loadMechanics(mechanicsFile.get());
          } else {
            appLog(describeComponentMissing(Component::Mechanics),
                   LogOrigin::Error);
            loadError = true;
          }
        }
      }

      workerRunning = false;
    });
  } else {
    appLog("Tried to start secondary loading before primary loading finished.", LogOrigin::Error);
  }
}

void Environment::joinWorker() {
  if (workerThread.joinable())
    workerThread.join();
}

bool Environment::loadingErrored() const {
  return loadError;
}

bool Environment::loadingIdle() const {
  return !workerRunning;
}

float Environment::loadingProgress() const {
  return loadProgress;
}

Map const *Environment::getMap() const {
  return map.get_ptr();
}

Visuals const *Environment::getVisuals() const {
  return visuals.get_ptr();
}

Mechanics const *Environment::getMechanics() const {
  return mechanics.get_ptr();
}

}
