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
 * Subsystem that provides debug information for an Arena / SkyHandle.
 */
#pragma once

#include "arena.hpp"
#include "skyhandle.hpp"
#include "util/printer.hpp"

namespace sky {

class DebugView : public Subsystem<Nothing> {
 private:
  const SkyHandle &skyHandle;
  optional<PID> playerID;

 protected:
  // Subsystem impl.
  // TODO: use mutable state?

 public:
  DebugView() = delete;
  DebugView(Arena &arena,
            const SkyHandle &skyHandle,
            const optional<PID> &player = {});

  // User API.
  void printArenaReport(Printer &p) const;
  void printSkyReport(Printer &p) const;

};

}
