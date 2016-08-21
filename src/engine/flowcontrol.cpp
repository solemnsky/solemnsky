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
#include "flowcontrol.hpp"
#include "util/methods.hpp"
#include "util/printer.hpp"

namespace sky {

/**
 * FlowStats.
 */

FlowStats::FlowStats(const TimeDiff inputJitter,
                     const TimeDiff maxDelay,
                     const TimeDiff minDelay) :
    inputJitter(inputJitter),
    maxDelay(maxDelay),
    minDelay(minDelay) { }

void FlowStats::printDebug(Printer &p) const {
  p.printLn("flow.inputJitter: " + printTimeDiff(inputJitter));
  p.printLn("flow.maxDelay: " + printTimeDiff(maxDelay));
  p.printLn("flow.minDelay: " + printTimeDiff(minDelay));
}

/**
 * FlowControl.
 */

FlowControl::FlowControl() :
    offsets(50) { }

TimeDiff FlowControl::determineDelay(const Time localtime, const Time timestamp) {
  offsets.push(localtime - timestamp);
  return 0.02f;
}

FlowStats FlowControl::getStats() const {
  return FlowStats(offsets.max() - offsets.min(), 0.02f, 0.02f);
}

}

