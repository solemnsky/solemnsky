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
    envIsNew(false) {
  if (initializer) environment.emplace(initializer.get());
}

SkyHandleInit SkyHandle::captureInitializer() const {
  if (sky) return SkyHandleInit{environment->url};
  else return SkyHandleInit{};
}

SkyHandleDelta SkyHandle::collectDelta() {
  if (environment and envIsNew) {
    envIsNew = false;
    return SkyHandleDelta{environment->url};
  } else return SkyHandleDelta{};
}

void SkyHandle::applyDelta(const SkyHandleDelta &delta) {
  if (delta) environment.emplace(delta.get());
}

void SkyHandle::start() {
  environment.emplace(arena.getNextEnv());
}

void SkyHandle::instantiateSky(const SkyInit &init) {
  assert(environment->getMap());
  sky.emplace(arena, *environment->getMap(), init);
}

void SkyHandle::stop() {
  environment.reset();
  sky.reset();
}

}

