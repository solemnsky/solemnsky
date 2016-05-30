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
#include "skyhandle.h"
#include "printer.h"

namespace sky {

/**
 * SkyHandleInit.
 */

SkyHandleInit::SkyHandleInit(
    const SkyInit &initializer) :
    initializer(initializer) { }

bool SkyHandleInit::verifyStructure() const {
  if (initializer) {
    return verifyValue(initializer->second);
  } else return false;
}

/**
 * SkyHandleDelta.
 */

bool SkyHandleDelta::verifyStructure() const {
  if (bool(initializer) and bool(delta)) return false;

  if (initializer) return verifyValue(initializer.get());
  if (delta) return verifyValue(delta.get());
  return true;
}

SkyHandleDelta SkyHandleDelta::respectAuthority(const Player &player) const {
  SkyHandleDelta newDelta;
  if (delta) newDelta.delta = delta->respectAuthority(player);
  newDelta.initializer = initializer;
  return newDelta;
}

/**
 * SkyHandle.
 */

SkyHandle::SkyHandle(Arena &arena, const SkyHandleInit &initializer) :
    Subsystem(arena),
    Networked(initializer),
    skyIsNew(false) {
  if (const auto &init = initializer.initializer) {
    auto loaded = Map::load(init->first);
    if (loaded) {
      map.emplace(loaded);
      sky.emplace(arena, map.get(), init->second);
    } else {
      appLog("could not initialize map or sky, something should happen",
             LogOrigin::Engine);
    }
  }
}

SkyHandleInit SkyHandle::captureInitializer() const {
  SkyHandleInit initializer;
  if (sky) {
    initializer.initializer.emplace(
        map->name,
        sky->captureInitializer());
  }
  return initializer;
}

SkyHandleDelta SkyHandle::collectDelta() {
  SkyHandleDelta delta;
  if (sky and skyIsNew) {
    delta.initializer.emplace(
        map->name,
        sky->captureInitializer());
    skyIsNew = false;
  }
  if (sky) delta.delta = sky->collectDelta();
  return delta;
}

void SkyHandle::applyDelta(const SkyHandleDelta &delta) {
  if (const auto init = delta.initializer) {
    map.emplace(init->first);
    sky.emplace(arena, map, init->second);
    caller.doStartGame();
  }
  if (sky) {
    if (delta.delta) sky->applyDelta(delta.delta.get());
    else {
      sky.reset();
      map.reset();
      caller.doEndGame();
    }
  }
}

void SkyHandle::start() {
  stop();
  map.emplace(arena.getNextMap());
  sky.emplace(arena, sky, SkyInit());
  skyIsNew = true;
  caller.doStartGame();
}

void SkyHandle::stop() {
  if (sky) sky.reset();
  caller.doEndGame();
}

bool SkyHandle::isActive() const {
  return bool(sky);
}

}

