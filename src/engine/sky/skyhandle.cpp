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
 * SkyHandle.
 */

SkyHandle::SkyHandle(Arena &arena, const SkyHandleInit &initializer) :
    Subsystem(arena),
    Networked(initializer),
    environment(),
    sky(),
    envStateIsNew(false) {
  if (initializer) environment.emplace(initializer.get());
}

Environment *SkyHandle::getEnvironment() {
  return environment.get_ptr();
}

Sky *SkyHandle::getSky() {
  return sky.get_ptr();
}

Environment const *SkyHandle::getEnvironment() const {
  return environment.get_ptr();
}

Sky const *SkyHandle::getSky() const {
  return sky.get_ptr();
}

bool SkyHandle::loadingToSky() const {
  return getEnvironment() and !getSky();
}

bool SkyHandle::readyToLoadSky() const {
  if (auto env = getEnvironment()) {
    return env->getMap() and !getSky();
  }
  return false;
}

SkyHandleInit SkyHandle::captureInitializer() const {
  if (environment) return SkyHandleInit{environment->url};
  else return SkyHandleInit{};
}

optional<SkyHandleDelta> SkyHandle::collectDelta() {
  if (envStateIsNew) {
    envStateIsNew = false;
    if (environment) {
      return SkyHandleDelta{environment->url};
    } else {
      return SkyHandleDelta{};
    }
  } else {
    return optional<SkyHandleDelta>();
  }
}

void SkyHandle::applyDelta(const SkyHandleDelta &delta) {
  if (delta) {
    environment.emplace(delta.get());
    caller.doStartGame();
  } else {
    environment.reset();
    caller.doEndGame();
  }
}

void SkyHandle::start() {
  envStateIsNew = true;
  environment.emplace(arena.getNextEnv());
  caller.doStartGame();
}

void SkyHandle::instantiateSky(const SkyInit &init) {
  assert(environment);
  assert(environment->getMap());

  sky.emplace(arena, *environment->getMap(), init);
}

void SkyHandle::stop() {
  envStateIsNew = true;
  environment.reset();
  sky.reset();
  caller.doEndGame();
}

}

