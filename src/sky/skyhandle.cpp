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

namespace sky {

/**
 * SkyHandleInit.
 */

SkyHandleInit::SkyHandleInit(
    const SkyInit &initializer) :
    initializer(initializer) { }

bool SkyHandleInit::verifyStructure() const {
  return verifyValue(initializer);
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

/**
 * SkyHandle.
 */

SkyHandle::SkyHandle(Arena &arena, const SkyHandleInit &initializer) :
    Subsystem(arena),
    Networked(initializer),
    skyIsNew(false) {
  if (const auto &skyInit = initializer.initializer) {
    sky.emplace(arena, skyInit.get());
  }
}

SkyHandleInit SkyHandle::captureInitializer() const {
  SkyHandleInit initializer;
  if (sky) {
    initializer.initializer = sky->captureInitializer();
  }
  return initializer;
}

SkyHandleDelta SkyHandle::collectDelta() {
  SkyHandleDelta delta;
  if (sky and skyIsNew) {
    delta.initializer = sky->captureInitializer();
    skyIsNew = false;
  }
  if (sky) delta.delta = sky->collectDelta();
  return delta;
}

void SkyHandle::applyDelta(const SkyHandleDelta &delta) {
  if (delta.initializer) {
    sky.emplace(arena, delta.initializer.get());
    caller.doStartGame();
  }
  if (sky) {
    if (delta.delta) sky->applyDelta(delta.delta.get());
    else {
      sky.reset();
      caller.doEndGame();
    }
  }
}

SkyHandleDelta SkyHandle::respectAuthority(const SkyHandleDelta &delta,
                                           const Player &player) const {
  SkyHandleDelta newDelta;
  if (sky and delta.delta) {
    newDelta.delta = sky->respectAuthority(delta.delta.get(), player);
  }
  newDelta.initializer = delta.initializer;
  return newDelta;
}

void SkyHandle::start() {
  stop();
  sky.emplace(arena, SkyInit(arena.getNextMap()));
  skyIsNew = true;
  caller.doStartGame();
}

void SkyHandle::stop() {
  if (sky) sky.reset();
  caller.doEndGame();
}

const optional<Sky> &SkyHandle::getSky() const {
  return sky;
}

bool SkyHandle::isActive() const {
  return bool(sky);
}

}

