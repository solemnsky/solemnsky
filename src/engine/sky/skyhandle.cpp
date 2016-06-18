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
#include "skyhandle.hpp"
#include "util/printer.hpp"

namespace sky {

/**
 * SkyHandleInit.
 */

SkyHandleInit::SkyHandleInit(
    const EnvironmentURL &environment) :
    environment(environment) {}

bool SkyHandleInit::verifyStructure() const {
  return true;
}

/**
 * SkyHandleDelta.
 */

bool SkyHandleDelta::verifyStructure() const {
  if (bool(initializer) and bool(delta)) return false;

  if (initializer) return verifyValue(initializer->second);
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
    envIsNew(false) {
  if (const auto &envUrl = initializer.environment) {
    environment.emplace(envUrl);
  }
}

SkyHandleInit SkyHandle::captureInitializer() const {
  SkyHandleInit initializer;
  if (sky) return SkyHandleInit{environment->url};
  else return SkyHandleInit{};
}

SkyHandleDelta SkyHandle::collectDelta() {
  SkyHandleDelta delta;
  if (environment and envIsNew) {
    return environment->url;
    envIsNew = false;
  }
  if (sky) delta.delta = sky->collectDelta();
  return delta;
}

void SkyHandle::applyDelta(const SkyHandleDelta &delta) {
  if (const auto init = delta.initializer) {
    startWith(init->first, init->second);
    return;
  }
  if (sky) {
    if (delta.delta) sky->applyDelta(delta.delta.get());
    else stop();
  }
}

void SkyHandle::start() {
  stop();
  startWith(arena.getNextMap(), SkyInit());
}

void SkyHandle::stop() {
  sky.reset();
  map.reset();
  loadError = false;
  caller.doEndGame();
}

bool SkyHandle::isActive() const {
  return bool(sky);
}

bool SkyHandle::loadingErrored() const {
  return loadError;
}

}

