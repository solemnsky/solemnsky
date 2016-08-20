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
#include "skydeltacache.hpp"

namespace sky {

SkyDeltaCache::SkyDeltaCache(Arena &arena, SkyHandle &skyHandle) :
    Subsystem(arena), skyHandle(skyHandle) {}

void SkyDeltaCache::receive(const Time timestamp, const SkyDelta &delta) {
  deltaControl.registerMessage(arena.getUptime(), timestamp, delta);
}

void SkyDeltaCache::onPoll() {
  if (auto sky = skyHandle.getSky()) {
    while (const auto delta = deltaControl.pull(arena.getUptime())) {
      sky->applyDelta(delta.get());
    }
  } else {
    deltaControl.reset();
  }
}

void SkyDeltaCache::onDebugRefresh() {
  stats.emplace(deltaControl.getStats());
}

void SkyDeltaCache::printDebug(Printer &p) {
  p.printTitle("SkyDeltaCache");
  if (stats) {
    p.printLn("average time lost in cache: " + printTimeDiff(stats->averageWait));
    p.printLn("actual offset jitter" + printTimeDiff(stats->totalJitter));
  } else {
    p.printLn("no stats collected yet...");
  }
}

}
