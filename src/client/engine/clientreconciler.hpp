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
/**
 * Manages the reconciliation of sky update packets.
 */
#pragma once
#include "engine/arena.hpp"
#include "engine/sky/skyhandle.hpp"
#include "engine/flowcontrol.hpp"
#include "util/printer.hpp"

namespace sky {

class SkyDeltaCache: public Subsystem<Nothing> {
 private:
  SkyHandle &skyHandle;
  FlowControl<SkyDelta> deltaControl;

  optional<FlowStats> stats;

 protected:
  // Subsystem impl.
  virtual void onPoll() override final;
  virtual void onDebugRefresh() override final;

 public:
  SkyDeltaCache(Arena &arena, SkyHandle &skyHandle);

  void receive(const Time timestamp, const SkyDelta &delta);

  void printDebug(Printer &p);

};

}
