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
#include "clientreconciler.hpp"

namespace sky {

SkyDeltaCache::SkyDeltaCache(Arena &arena, SkyHandle &skyHandle) :
    Subsystem(arena), skyHandle(skyHandle) {}

void SkyDeltaCache::receive(const Time timestamp, const SkyDelta &delta) {
}

void SkyDeltaCache::onPoll() {
}

void SkyDeltaCache::onDebugRefresh() {
  stats.emplace(deltaControl.getStats());
}

void SkyDeltaCache::printDebug(Printer &p) {
  p.printTitle("SkyDeltaCache");
  if (stats) {
    p.printLn("stats.averageWait: " + printTimeDiff(stats->averageWait));
    p.printLn("stats.inputJitter: " + printTimeDiff(stats->inputJitter));
    p.printLn("stats.outputJitter: " + printTimeDiff(stats->outputJitter));
  } else {
    p.printLn("no stats collected yet...");
  }
}

}
